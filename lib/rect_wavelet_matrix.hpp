#ifndef LIB_RECT_WAVELET_MATRIX_HPP
#define LIB_RECT_WAVELET_MATRIX_HPP 1

#include <algorithm>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>
#include <lib/segment_tree.hpp>
#include <lib/bit_vector.hpp>

template <typename T, typename Monoid, is_commutative_monoid_t<Monoid>* = nullptr>
struct rect_wavelet_matrix {
	using MX = Monoid;
	using X = typename MX::ValueT;

	using pt = std::pair<T, T>;

        i32 n, size, log;
	std::vector<pt> pts;
	std::vector<T> ys;
        std::vector<i32> md;
	std::vector<segment_tree<Monoid>> sg;
        std::vector<bit_vector> bv;

        rect_wavelet_matrix() {}

	explicit rect_wavelet_matrix(i32 m) {
		pts.reserve(m);
		ys.reserve(m);
	}

	void add_point(T x, T y) {
		pts.emplace_back(x, y);
		ys.push_back(y);
	}

        void build() {
		std::sort(pts.begin(), pts.end());
		pts.erase(std::unique(pts.begin(), pts.end()), pts.end());

                n = static_cast<i32>(pts.size());

		std::sort(ys.begin(), ys.end());
		ys.erase(std::unique(ys.begin(), ys.end()), ys.end());

		ys.shrink_to_fit();
		size = static_cast<i32>(ys.size());

		std::vector<i32> b(n);
		for (i32 i = 0; i < n; ++i) b[i] = yid(pts[i].second);

                log = 0;
                while ((1 << log) < size) ++log;

                md.resize(log);
                bv.assign(log, bit_vector(n));

		for (i32 i = 0; i <= log; ++i) sg.emplace_back(n);

                std::vector<i32> b0(n), b1(n);
                for (i32 d = log - 1; d >= 0; --d) {
                        i32 p0{}, p1{};
                        for (i32 i = 0; i < n; ++i) {
                                if ((b[i] >> d) & 1) {
                                        bv[d].set(i);
                                        b1[p1] = b[i];
                                        ++p1;
                                } else {
                                        b0[p0] = b[i];
                                        ++p0;
                                }
                        }

                        std::swap(b, b0);
                        std::move(b1.begin(), b1.begin() + p1, b.begin() + p0);

                        md[d] = p0;
                        bv[d].build();
                }
        }

	X prod(T xl, T xr, T yl, T yr) const {
		const i32 l = xid(xl);
		const i32 r = xid(xr);

                const i32 lo = yid(yl);
                const i32 hi = yid(yr);

                X x = MX::unit();

                const auto dfs = [&](auto &&self, i32 d, i32 L, i32 R, i32 p, i32 q) -> void {
			assert((q - p) == (1 << d));

                        if (hi <= p || q <= lo) return;
                        if (lo <= p && q <= hi) {
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
                return x;
        }

        void set(T p, T q, X x) {
		const i32 i = static_cast<i32>(std::lower_bound(pts.begin(), pts.end(), pt{p, q}) - pts.begin());
		assert(i < n && pts[i].first == p && pts[i].second == q);

                i32 l = i;
                i32 r = i + 1;

                sg[log].set(l, x);
                for (i32 d = log - 1; d >= 0; --d) {
                        const i32 l0 = bv[d].rank0(l);
                        const i32 r0 = bv[d].rank0(r);

                        const i32 l1 = l + md[d] - l0;
                        const i32 r1 = r + md[d] - r0;

                        if (l0 < r0) {
                                l = l0;
                                r = r0;
                        } else {
                                l = l1;
                                r = r1;
                        }

                        sg[d].set(l, x);
                }
        }

	void multiply(T p, T q, X x) {
		const i32 i = static_cast<i32>(std::lower_bound(pts.begin(), pts.end(), pt{p, q}) - pts.begin());
		assert(i < n && pts[i].first == p && pts[i].second == q);

                i32 l = i;
                i32 r = i + 1;

                sg[log].multiply(l, x);
                for (i32 d = log - 1; d >= 0; --d) {
                        const i32 l0 = bv[d].rank0(l);
                        const i32 r0 = bv[d].rank0(r);

                        const i32 l1 = l + md[d] - l0;
                        const i32 r1 = r + md[d] - r0;

                        if (l0 < r0) {
                                l = l0;
                                r = r0;
                        } else {
                                l = l1;
                                r = r1;
                        }

                        sg[d].multiply(l, x);
                }
        }

private:
	X prod(i32 l, i32 r, T y) const {
		const i32 p = yid(y);

                X x = MX::unit();
                for (i32 d = log - 1; d >= 0; --d) {
                        const i32 l0 = bv[d].rank0(l);
                        const i32 r0 = bv[d].rank0(r);

                        const i32 l1 = l + md[d] - l0;
                        const i32 r1 = r + md[d] - r0;

                        if ((p >> d) & 1) {
                                x = MX::op(x, sg[d].prod(l0, r0));

                                l = l1;
                                r = r1;
                        } else {
                                l = l0;
                                r = r0;
                        }
                }

                return x;
        }

	inline i32 xid(i32 x) const {
		const auto cmp = [](const pt& a, const pt& b) { return a.first < b.first; };
		return static_cast<i32>(std::lower_bound(pts.begin(), pts.end(), pt{x, MX::unit()}, cmp) - pts.begin());
	}

	inline i32 yid(i32 y) const {
		return static_cast<i32>(std::lower_bound(ys.begin(), ys.end(), y) - ys.begin());
	}
};

#endif // LIB_RECT_WAVELET_MATRIX_HPP
