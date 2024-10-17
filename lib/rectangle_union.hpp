#ifndef LIB_RECTANGLE_UNION_HPP
#define LIB_RECTANGLE_UNION_HPP 1

#include <vector>
#include <cassert>
#include <numeric>
#include <algorithm>

#include <lib/prelude.hpp>
#include <lib/acted_monoids/min_cnt_add.hpp>
#include <lib/lazy_segment_tree.hpp>

template <typename T>
T rectangle_union(const std::vector<std::array<T, 4>> &rs) {
	// x0, y0, x1, y1
	const i32 n = static_cast<i32>(rs.size()) * 2;

	std::vector<T> xs(n), ys(n);
	for (i32 i = 0; i < n; ++i) {
		xs[i] = rs[i / 2][0 + 2 * (i % 2)];
		ys[i] = rs[i / 2][1 + 2 * (i % 2)];
	}

	std::vector<i32> xi(n), yi(n), yr(n);
	std::iota(xi.begin(), xi.end(), 0);
	std::iota(yi.begin(), yi.end(), 0);

	std::sort(xi.begin(), xi.end(), [&](const i32 a, const i32 b) { 
		return xs[a] == xs[b] ? a < b : xs[a] < xs[b]; 
	});

	std::sort(yi.begin(), yi.end(), [&](const i32 a, const i32 b) { 
		return ys[a] == ys[b] ? a < b : ys[a] < ys[b]; 
	});

	std::vector<T> as(n), bs(n);
	for (i32 i = 0; i < n; ++i) {
		as[i] = xs[xi[i]];
		bs[i] = ys[yi[i]];
		yr[yi[i]] = i;
	}

	lazy_segment_tree<acted_monoid_min_cnt_add<i32, T>> st(n - 1, 
		[&](const i32 i) -> std::pair<i32, T> { return {0, bs[i + 1] - bs[i]}; 	});

	T area{};
	T total = bs[n - 1] - bs[0];

	for (i32 i = 0; i < n - 1; ++i) {
		const i32 k = xi[i] / 2;
		const i32 a = xi[i] % 2 ? -1 : 1;

		const auto [l, r] = std::minmax(yr[2 * k], yr[2 * k + 1]);
		st.apply(l, r, a);

		const auto [m, c] = st.prod_all();

		const T dy = total - (m == 0 ? c : 0);
		const T dx = as[i + 1] - as[i];

		area += dx * dy;
	}

	return area;
};

#endif // LIB_RECTANGLE_UNION_HPP
