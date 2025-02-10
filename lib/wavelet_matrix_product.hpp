#ifndef LIB_WAVELET_MATRIX_PRODUCT_HPP
#define LIB_WAVELET_MATRIX_PRODUCT_HPP 1

#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>

#include <lib/bit_vector.hpp>
#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename T, typename RangeProduct, is_commutative_monoid_t<typename RangeProduct::MX> * = nullptr>
struct wavelet_matrix_product {
    using MX = typename RangeProduct::MX;
    using X = typename MX::ValueT;

    i32 n, size, log;
    std::vector<T> rv;
    std::vector<i32> md;
    std::vector<bit_vector> bv;
    std::vector<RangeProduct> sg;

    wavelet_matrix_product() {}
    explicit wavelet_matrix_product(const std::vector<T> &v, std::vector<X> s) {
        build(v, std::move(s));
    }

    void build(const std::vector<T> &v, std::vector<X> s) {
        n = static_cast<i32>(v.size());

        std::vector<i32> vi(n);
        std::iota(vi.begin(), vi.end(), 0);
        std::sort(vi.begin(), vi.end(), [&](const i32 i, const i32 j) { return v[i] == v[j] ? i < j : v[i] < v[j]; });

        rv.reserve(n);
        std::vector<i32> b(n);

        for (const i32 i : vi) {
            if (rv.empty() || rv.back() != v[i]) rv.push_back(v[i]);
            b[i] = static_cast<i32>(rv.size()) - 1;
        }

        rv.shrink_to_fit();
        size = static_cast<i32>(rv.size());

        log = 0;
        while ((1 << log) < size) ++log;

        md.resize(log);
        bv.assign(log, bit_vector(n));

        sg.resize(log + 1);
        sg[log].build(s);

        std::vector<X> s0(n), s1(n);
        std::vector<i32> b0(n), b1(n);

        for (i32 d = log - 1; d >= 0; --d) {
            i32 p0{}, p1{};
            for (i32 i = 0; i < n; ++i) {
                if ((b[i] >> d) & 1) {
                    bv[d].set(i);
                    b1[p1] = b[i];
                    s1[p1] = s[i];
                    ++p1;
                } else {
                    b0[p0] = b[i];
                    s0[p0] = s[i];
                    ++p0;
                }
            }

            std::swap(b, b0);
            std::swap(s, s0);
            std::move(s1.begin(), s1.begin() + p1, s.begin() + p0);
            std::move(b1.begin(), b1.begin() + p1, b.begin() + p0);

            md[d] = p0;
            bv[d].build();
            sg[d].build(s);
        }
    }

    std::pair<i32, X> count(i32 l, i32 r, T a) const {
        assert(0 <= l && l <= r && r <= n);

        const i32 p = static_cast<i32>(std::lower_bound(rv.begin(), rv.end(), a) - rv.begin());

        if (l == r || p == 0) return {0, MX::unit()};
        if (p == size) return {r - l, sg[log].prod(l, r)};

        i32 cnt{};
        X x = MX::unit();
        for (i32 d = log - 1; d >= 0; --d) {
            const i32 l0 = bv[d].rank0(l);
            const i32 r0 = bv[d].rank0(r);

            const i32 l1 = l + md[d] - l0;
            const i32 r1 = r + md[d] - r0;

            if ((p >> d) & 1) {
                cnt += r0 - l0;
                x = MX::op(x, sg[d].prod(l0, r0));
                l = l1;
                r = r1;
            } else {
                l = l0;
                r = r0;
            }
        }

        return {cnt, x};
    }

    std::pair<i32, X> count(i32 l, i32 r, T a, T b) const {
        if constexpr (has_inv_v<MX>) {
            const auto [c1, x1] = count(l, r, a);
            const auto [c2, x2] = count(l, r, b);
            return {c2 - c1, MX::op(MX::inv(x1), x2)};
        }

        const i32 lo = static_cast<i32>(std::lower_bound(rv.begin(), rv.end(), a) - rv.begin());
        const i32 hi = static_cast<i32>(std::lower_bound(rv.begin(), rv.end(), b) - rv.begin());

        i32 cnt{};
        X x = MX::unit();
        const auto dfs = [&](auto &&self, i32 d, i32 L, i32 R, i32 p, i32 q) -> void {
            assert(q - p == 1 << d);

            if (hi <= p || q <= lo) return;
            if (lo <= p && q <= hi) {
                cnt += R - L;
                x = MX::op(x, sg[d].prod(L, R));
                return;
            }

            --d;
            const i32 c = (p + q) / 2;

            const i32 l0 = bv[d].rank0(L);
            const i32 r0 = bv[d].rank0(R);

            const i32 l1 = L + md[d] - l0;
            const i32 r1 = R + md[d] - r0;

            self(self, d, l0, r0, p, c);
            self(self, d, l1, r1, c, q);
        };

        dfs(dfs, log, l, r, 0, 1 << log);
        return {cnt, x};
    }

    std::pair<T, X> kth(i32 l, i32 r, i32 k) const {
        assert(0 <= l && l <= r && r <= n);
        assert(0 <= k && k < r - l);

        i32 p{};
        X x = MX::unit();
        for (i32 d = log - 1; d >= 0; --d) {
            const i32 l0 = bv[d].rank0(l);
            const i32 r0 = bv[d].rank0(r);

            const i32 l1 = l + md[d] - l0;
            const i32 r1 = r + md[d] - r0;

            if (k < r0 - l0) {
                l = l0;
                r = r0;
            } else {
                k -= r0 - l0;
                l = l1;
                r = r1;
                p |= 1 << d;
                x = MX::op(x, sg[d].prod(l0, r0));
            }
        }

        x = MX::op(x, sg[0].prod(l, l + k));
        return {rv[p], x};
    }

    std::pair<T, X> kth(std::vector<std::pair<i32, i32>> segments, i32 k) const {
        i32 cnt{}, p{};
        X x = MX::unit();
        for (i32 d = log - 1; d >= 0; --d) {
            i32 c = 0;
            for (const auto &[l, r] : segments) {
                const i32 l0 = bv[d].rank0(l);
                const i32 r0 = bv[d].rank0(r);
                c += r0 - l0;
            }

            if (cnt + c > k) {
                for (auto &&[l, r] : segments) {
                    const i32 l0 = bv[d].rank0(l);
                    const i32 r0 = bv[d].rank0(r);

                    l = l0;
                    r = r0;
                }
            } else {
                cnt += c;
                p |= 1 << d;

                for (auto &&[l, r] : segments) {
                    const i32 l0 = bv[d].rank0(l);
                    const i32 r0 = bv[d].rank0(r);

                    x = MX::op(x, sg[d].prod(l0, r0));

                    l += md[d] - l0;
                    r += md[d] - r0;
                }
            }
        }

        for (const auto &[l, r] : segments) {
            const i32 t = std::min(r - l, k - cnt);

            x = MX::op(x, sg[0].prod(l, l + t));
            cnt += t;
        }

        return {rv[p], x};
    }

    void set(i32 p, const X &x) {
        assert(0 <= p && p < n);

        i32 l = p;
        i32 r = p + 1;

        sg[log].set(l, x);
        for (i32 d = log - 1; d >= 0; --d) {
            const i32 l0 = bv[d].rank0(l);
            const i32 r0 = bv[d].rank0(r);

            const i32 l1 = l + md[d] - l0;
            const i32 r1 = r + md[d] - r0;

            if (l0 < r0) {
                l = l0;
                r = r0;
            } else if (l0 == r0) {
                l = l1;
                r = r1;
            }

            sg[d].set(l, x);
        }
    }

    void multiply(i32 p, const X &x) {
        assert(0 <= p && p < n);

        i32 l = p;
        i32 r = p + 1;

        sg[log].multiply(l, x);
        for (i32 d = log - 1; d >= 0; --d) {
            const i32 l0 = bv[d].rank0(l);
            const i32 r0 = bv[d].rank0(r);

            const i32 l1 = l + md[d] - l0;
            const i32 r1 = r + md[d] - r0;

            if (l0 < r0) {
                l = l0;
                r = r0;
            } else if (l0 == r0) {
                l = l1;
                r = r1;
            }

            sg[d].multiply(l, x);
        }
    }
};

#endif // LIB_WAVELET_MATRIX_PRODUCT_HPP
