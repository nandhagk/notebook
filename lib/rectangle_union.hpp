#ifndef LIB_RECTANGLE_UNION_HPP
#define LIB_RECTANGLE_UNION_HPP 1

#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>

#include <lib/acted_monoids/min_cnt_add.hpp>
#include <lib/lazy_segment_tree.hpp>
#include <lib/prelude.hpp>

template <typename T>
struct rectangle_union {
    std::vector<T> xs, ys;

    rectangle_union() {}
    explicit rectangle_union(i32 n) {
        xs.reserve(2 * n);
        ys.reserve(2 * n);
    }

    void add_rectangle(T x0, T y0, T x1, T y1) {
        xs.push_back(x0);
        xs.push_back(x1);

        ys.push_back(y0);
        ys.push_back(y1);
    }

    T area() {
        const i32 n = static_cast<i32>(xs.size());

        std::vector<i32> xi(n), yi(n), yr(n);
        std::iota(xi.begin(), xi.end(), 0);
        std::iota(yi.begin(), yi.end(), 0);

        std::sort(xi.begin(), xi.end(),
                  [&](const i32 a, const i32 b) { return xs[a] == xs[b] ? a < b : xs[a] < xs[b]; });

        std::sort(yi.begin(), yi.end(),
                  [&](const i32 a, const i32 b) { return ys[a] == ys[b] ? a < b : ys[a] < ys[b]; });

        std::vector<T> as(n), bs(n);
        for (i32 i = 0; i < n; ++i) {
            as[i] = xs[xi[i]];
            bs[i] = ys[yi[i]];
            yr[yi[i]] = i;
        }

        lazy_segment_tree<acted_monoid_min_cnt_add<i32, T>> st(
            n - 1, [&](const i32 i) -> std::pair<i32, T> { return {0, bs[i + 1] - bs[i]}; });

        T ans{};
        T total = bs[n - 1] - bs[0];

        for (i32 i = 0; i < n - 1; ++i) {
            const i32 k = xi[i] / 2;
            const i32 a = xi[i] % 2 ? -1 : 1;

            const auto [l, r] = std::minmax(yr[2 * k], yr[2 * k + 1]);
            st.apply(l, r, a);

            const auto [m, c] = st.prod_all();

            const T dy = total - (m == 0 ? c : 0);
            const T dx = as[i + 1] - as[i];

            ans += dx * dy;
        }

        return ans;
    }
};

#endif // LIB_RECTANGLE_UNION_HPP
