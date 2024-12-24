#ifndef LIB_TECC_HPP
#define LIB_TECC_HPP 1

#include <algorithm>
#include <vector>

#include <lib/dsu.hpp>
#include <lib/prelude.hpp>

template <typename Graph> inline std::pair<i32, std::vector<i32>> tecc(const Graph &g) {
    const i32 n = static_cast<i32>(g.size());

    i32 dft = -1;
    std::vector<i32> pre(n, -1), post(n), path(n, -1), low(n), deg(n);

    dsu dsu(n);
    const auto dfs = [&](auto &&self, i32 u, i32 t = -1) -> void {
        i32 pc{};

        low[u] = pre[u] = ++dft;
        for (i32 v : g[u]) {
            if (v == u || (v == t && !pc++)) continue;

            if (pre[v] != -1) {
                if (pre[v] < pre[u]) {
                    ++deg[u];
                    low[u] = std::min(low[u], pre[v]);
                    continue;
                }

                --deg[u];
                for (i32 &p = path[u]; p != -1 && pre[p] <= pre[v] && pre[v] <= post[p]; p = path[p]) {
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

        post[u] = dft;
    };

    for (i32 u = 0; u < n; ++u)
        if (pre[u] == -1) dfs(dfs, u);

    return dsu.ids();
}

#endif // LIB_TECC_HPP
