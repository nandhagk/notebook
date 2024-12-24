#ifndef LIB_BECC_HPP
#define LIB_BECC_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>

template <typename Graph> inline std::pair<i32, std::vector<i32>> becc(const Graph &g) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<i32> dep(n, -1), cnt(n), par(n), ids(n), vs;
    vs.reserve(n);

    const auto dfs = [&](auto &&self, i32 u) -> void {
        vs.push_back(u);

        for (const i32 v : g[u]) {
            if (dep[v] == -1) {
                dep[v] = dep[u] + 1;
                par[v] = u;
                self(self, v);
                cnt[u] += cnt[v];
            } else if (dep[v] > dep[u]) {
                --cnt[u];
            } else if (dep[v] < dep[u]) {
                ++cnt[u];
            } else {
                assert(u == v);
            }
        }

        --cnt[u];
    };

    for (i32 u = 0; u < n; ++u) {
        if (dep[u] != -1) continue;

        dep[u] = 0;
        dfs(dfs, u);
    }

    i32 group{};
    for (const i32 u : vs) ids[u] = dep[u] && cnt[u] ? ids[par[u]] : group++;

    return {group, std::move(ids)};
}

#endif // LIB_BECC_HPP
