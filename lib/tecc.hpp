#ifndef LIB_TECC_HPP
#define LIB_TECC_HPP 1

#include <vector>
#include <algorithm>

#include <lib/prelude.hpp>
#include <lib/dsu.hpp>

inline std::vector<i32> tecc(const std::vector<std::vector<i32>> &g) {
        const i32 n = static_cast<i32>(g.size());

        i32 time{};
        std::vector<i32> tin(n, -1), tout(n), path(n, -1), low(n), deg(n);

        dsu dsu(n);
        const auto dfs = [&](auto &&self, i32 u, i32 t = -1) -> void {
                tin[u] = low[u] = time++;

                i32 cnt{};
                for (i32 v : g[u]) {
                        if (v == u || (v == t && !cnt++)) continue;

                        if (tin[v] != -1) {
                                if (tin[v] < tin[u]) {
                                        ++deg[u];
                                        low[u] = std::min(low[u], tin[v]);
                                        continue;
                                }

                                --deg[u];
                                for (i32 &p = path[u]; p != -1 && tin[p] <= tin[v] && tin[v] <= tout[p]; p = path[p]) {
                                        dsu.merge(u, p);
                                        deg[u] += deg[p];
                                }

                                continue;
                        }

                        self(self, v, u);
                        if (path[v] == -1 && deg[v] <= 1) {
                                low[u] = std::min(low[u], low[v]);
                                deg[u] += deg[v]; 
                                continue;
                        }

                        if (deg[v] == 0) v = path[v];
                        if (low[u] > low[v]) {
                                low[u] = std::min(low[u], low[v]);
                                std::swap(v, path[u]);
                        }

                        for (; v != -1; v = path[v]) {
                                dsu.merge(u, v);
                                deg[u] += deg[v];
                        }
                }

                tout[u] = time;
        };

        for (i32 u = 0; u < n; ++u) {
                if (tin[u] == -1) dfs(dfs, u);
        }

        return dsu.ids();
}

#endif // LIB_TECC_HPP
