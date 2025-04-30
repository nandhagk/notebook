#ifndef LIB_ENUMERATE_CLIQUES_HPP
#define LIB_ENUMERATE_CLIQUES_HPP 1

#include <vector>

#include <lib/prelude.hpp>

template <typename Graph, typename F>
void enumerate_cliques(const Graph &g, F f) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<i32> deg(n);
    for (i32 u = 0; u < n; ++u) deg[u] = static_cast<i32>(g[u].size());

    std::vector<bool> done(n);
    std::vector<std::vector<bool>> can(n, std::vector<bool>(n));

    for (i32 u = 0; u < n; ++u)
        for (const i32 v : g[u]) can[u][v] = can[v][u] = true;

    for (i32 i = 0; i < n; ++i) {
        i32 v = -1, m = n;
        for (i32 u = 0; u < n; ++u) {
            if (!done[u] && deg[u] < m) {
                v = u;
                m = deg[u];
            }
        }

        std::vector<i32> nbd;
        for (const i32 u : g[v])
            if (!done[u]) nbd.push_back(u);

        std::vector<i32> c{v};

        const auto dfs = [&](auto &&self, i32 k) -> void {
            f(c);

            for (i32 j = k; j < static_cast<i32>(nbd.size()); ++j) {
                bool ok = true;
                for (const i32 u : c) {
                    if (!can[u][nbd[j]]) {
                        ok = false;
                        break;
                    }
                }

                if (ok) {
                    c.push_back(nbd[j]);
                    self(self, j + 1);
                    c.pop_back();
                }
            }
        };

        dfs(dfs, 0);
        done[v] = true;

        for (const i32 u : g[v]) --deg[u];
    }
}

#endif // LIB_ENUMERATE_CLIQUES_HPP
