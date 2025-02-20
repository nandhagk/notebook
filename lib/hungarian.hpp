#ifndef LIB_HUNGARIAN_HPP
#define LIB_HUNGARIAN_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>

template <typename T>
std::vector<i32> hungarian(const std::vector<std::vector<T>> &cost) {
    const i32 n = static_cast<i32>(cost.size());
    const i32 m = static_cast<i32>(cost[0].size());

    std::vector<T> u(n + 1), v(m + 1);
    std::vector<i32> p(m + 1), way(m + 1);

    for (i32 i = 1; i <= n; ++i) {
        p[0] = i;
        i32 j0{};

        std::vector<T> minv(m + 1, inf<T>);
        std::vector<bool> used(m + 1, false);

        do {
            used[j0] = true;

            i32 i0 = p[j0];
            T delta = inf<T>;

            i32 j1{};
            for (i32 j = 1; j <= m; ++j) {
                if (used[j]) continue;

                const T cur = cost[i0 - 1][j - 1] - u[i0] - v[j];
                if (cur < minv[j]) {
                    minv[j] = cur;
                    way[j] = j0;
                }

                if (minv[j] < delta) {
                    delta = minv[j];
                    j1 = j;
                }
            }

            for (i32 j = 0; j <= m; ++j) {
                if (used[j]) {
                    u[p[j]] += delta;
                    v[j] -= delta;
                } else {
                    minv[j] -= delta;
                }
            }

            j0 = j1;
        } while (p[j0] != 0);

        do {
            i32 j1 = way[j0];
            p[j0] = p[j1];
            j0 = j1;
        } while (j0);
    }

    std::vector<i32> ans(n);
    for (i32 j = 1; j <= m; ++j) ans[p[j] - 1] = j - 1;

    return ans;
}

#endif // LIB_HUNGARIAN_HPP
