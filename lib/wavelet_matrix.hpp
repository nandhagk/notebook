#ifndef LIB_WAVELET_MATRIX_HPP
#define LIB_WAVELET_MATRIX_HPP 1

#include <vector>
#include <algorithm>
#include <numeric>
#include <cassert>

#include <lib/prelude.hpp>
#include <lib/bit_vector.hpp>

template <typename T>
struct wavelet_matrix {
        i32 n, size, log;
        std::vector<T> rv;
        std::vector<i32> md;
        std::vector<bit_vector> bv;

        wavelet_matrix() {}
        explicit wavelet_matrix(const std::vector<T> &v) {
                build(v);
        }

        void build(const std::vector<T> &v) {
                n = static_cast<i32>(v.size());

                std::vector<i32> vi(n);
                std::iota(vi.begin(), vi.end(), 0);
                std::sort(vi.begin(), vi.end(), [&](const i32 i, const i32 j) {
                        return v[i] == v[j] ? i < j : v[i] < v[j];
                });

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

                std::vector<i32> b0(n), b1(n);
                for (i32 d = log - 1; d >= 0; --d) {
                        i32 p0{}, p1{};
                        for (i32 i = 0; i < n; ++i) {
                                if ((b[i] >> d) & 1) {
                                        bv[d].set(i);
                                        b1[p1++] = b[i];
                                } else {
                                        b0[p0++] = b[i];
                                }
                        }

                        std::swap(b, b0);
                        std::move(b1.begin(), b1.begin() + p1, b.begin() + p0);

                        md[d] = p0;
                        bv[d].build();
                }
        }

        i32 count(i32 l, i32 r, T a) const {
                assert(0 <= l && l <= r && r <= n);

                const i32 p = static_cast<i32>(std::lower_bound(rv.begin(), rv.end(), a) - rv.begin());

                if (l == r || p == 0) return 0;
                if (p == size) return r - l;

                i32 cnt{};
                for (i32 d = log - 1; d >= 0; --d) {
                        const i32 l0 = bv[d].rank0(l);
                        const i32 r0 = bv[d].rank0(r);

                        const i32 l1 = l + md[d] - l0;
                        const i32 r1 = r + md[d] - r0;

                        if ((p >> d) & 1) {
                                cnt += r0 - l0;
                                l = l1;
                                r = r1;
                        } else {
                                l = l0;
                                r = r0;
                        }
                }

                return cnt;
        }

        i32 count(i32 l, i32 r, T a, T b) const {
                return count(l, r, a) - count(l, r, b);
        }

        T kth(i32 l, i32 r, i32 k) const {
                assert(0 <= l && l <= r && r <= n);
                assert(0 <= k && k < r - l);

                i32 p{};
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
                        }
                }

                return rv[p];
        }

        T next(i32 l, i32 r, T a) const {
                const auto it = std::upper_bound(rv.begin(), rv.end(), a);
                if (it == rv.end()) return a;

                const i32 k = count(l, r, *it);
                return k == r - l ? a : kth(l, r, k);
        }

        T prev(i32 l, i32 r, T a) const {
                const auto it = std::lower_bound(rv.begin(), rv.end(), a);
                if (it == rv.begin()) return a;

                const i32 k = count(l, r, *it);
                return k == 0 ? a : kth(l, r, k - 1);
        }
};

#endif // LIB_WAVELET_MATRIX_HPP
