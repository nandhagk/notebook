#ifndef LIB_RECTANGLE_UNION_HPP
#define LIB_RECTANGLE_UNION_HPP 1

#include <vector>
#include <cassert>
#include <numeric>
#include <algorithm>

#include <lib/prelude.hpp>
#include <lib/acted_monoids/min_cnt_add.hpp>
#include <lib/lazy_segment_tree.hpp>

struct rectangle_union {
	std::vector<i64> xs, ys;

	void add(i64 xl, i64 yt, i64 xr, i64 yb) {
		assert(xl < xr && yt > yb);

		xs.push_back(xl);
		xs.push_back(xr);

		ys.push_back(yt);
		ys.push_back(yb);
	}

	i64 area() {
		const i32 n = static_cast<i32>(xs.size());

		std::vector<i32> xi(n), yi(n), yr(n);
		std::iota(xi.begin(), xi.end(), 0);
		std::iota(yi.begin(), yi.end(), 0);

		std::sort(xi.begin(), xi.end(), [&](const i32 a, const i32 b) { 
			return xs[a] == xs[b] ? a < b : xs[a] < xs[b]; 
		});

		std::sort(yi.begin(), yi.end(), [&](const i32 a, const i32 b) { 
			return ys[a] == ys[b] ? a < b : ys[a] < ys[b]; 
		});

		std::vector<i64> as(n), bs(n);
		for (i32 i = 0; i < n; ++i) {
			as[i] = xs[xi[i]];
			bs[i] = ys[yi[i]];
			yr[yi[i]] = i;
		}

		lazy_segment_tree<acted_monoid_min_cnt_add<i64>> st(n - 1, [&](const i32 i) -> std::pair<i32, i64> { 
			return {0, bs[i + 1] - bs[i]}; 
		});

		i64 area{};
		i64 total = bs[n - 1] - bs[0];

		for (i32 i = 0; i < n - 1; ++i) {
			const i32 k = xi[i] / 2;
			const i32 a = xi[i] % 2 ? -1 : 1;

			const auto [l, r] = std::minmax(yr[2 * k], yr[2 * k + 1]);
			st.apply(l, r, a);

			const auto [m, c] = st.prod_all();
			const i64 dy = total - (m == 0 ? c : 0);
			const i64 dx = as[i + 1] - as[i];

			area += dx * dy;
		}

		return area;
	}
};

#endif // LIB_RECTANGLE_UNION_HPP
