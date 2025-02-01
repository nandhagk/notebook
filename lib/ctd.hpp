#ifndef LIB_CTD_HPP
#define LIB_CTD_HPP 1

#include <vector>

#include <lib/prelude.hpp>

template <typename Graph>
inline std::vector<i32> centroids(const Graph &g) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<i32> ctds;
    std::vector<i32> sz(n, 1);
    const auto dfs = [&](auto &&self, i32 u, i32 t = -1) -> void {
        bool flag = false;
        for (const i32 v : g[u]) {
            if (v == t) continue;

            self(self, v, u);
            if (sz[v] > n / 2) flag = true;
            sz[u] += sz[v];
        }

        if (flag) return;
        if (n - sz[u] > n / 2) return;

        ctds.push_back(u);
    };

    dfs(dfs, 0);
    return ctds;
}

#endif // LIB_CTD_HPP
