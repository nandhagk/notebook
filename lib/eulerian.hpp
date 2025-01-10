#ifndef LIB_EULERIAN_HPP
#define LIB_EULERIAN_HPP 1

#include <vector>
#include <optional>
#include <algorithm>
#include <numeric>

#include <lib/prelude.hpp>

template <typename Graph>
inline std::optional<std::pair<std::vector<i32>, std::vector<i32>>> eulerian_trail_directed(const Graph &g) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<i32> in(n), out(n);
    for (i32 u = 0; u < n; ++u) {
        out[u] = static_cast<i32>(g[u].size());
        for (const auto &[v, _] : g[u]) ++in[v];
    }

    i32 s = -1;
    for (i32 u = 0; u < n; ++u) {
        if (s == -1 && out[u]) s = u;
        if (out[u] > in[u]) s = u;
    }

    if (s == -1) return std::make_pair(std::vector<i32>{0}, std::vector<i32>{});

    std::vector<i32> us, vs, es;
    const auto dfs = [&](auto &&self, i32 u) -> void {
        while (out[u]) {
            const auto &[v, i] = g[u][--out[u]];

            --in[v];
            self(self, v);
            
            es.push_back(i);
            us.push_back(u);
            vs.push_back(v);
        }
    };

    dfs(dfs, s);

    std::reverse(us.begin(), us.end());
    std::reverse(vs.begin(), vs.end());
    std::reverse(es.begin(), es.end());

    for (i32 u = 0; u < n; ++u) if (in[u] || out[u]) return std::nullopt;
    for (i32 i = 1; i < static_cast<i32>(us.size()); ++i)
        if (vs[i - 1] != us[i]) return std::nullopt;

    us.push_back(vs.back());
    return std::make_pair(std::move(us), std::move(es));
}

template <typename Graph>
inline std::optional<std::pair<std::vector<i32>, std::vector<i32>>> eulerian_trail_undirected(const Graph& g) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<i32> deg(n);
    for (i32 u = 0; u < n; ++u) 
        deg[u] = static_cast<i32>(g[u].size());

    i32 s = -1;
    for (i32 u = 0; u < n; ++u) {
        if (s == -1 && deg[u]) s = u;
        if (deg[u] & 1) s = u;
    }

    if (s == -1) return std::make_pair(std::vector<i32>{0}, std::vector<i32>{});

    std::vector<i32> us, vs, es;
    auto pos = deg;

    const i32 m = std::reduce(deg.begin(), deg.end()) / 2;
    std::vector<bool> used(m);

    const auto dfs = [&](auto &&self, i32 u) -> void {
        while (deg[u]) {
            const auto &[v, i] = g[u][--pos[u]]; 
            if (used[i]) continue;

            --deg[u];
            --deg[v];
            used[i] = true;
            
            self(self, v);

            us.push_back(u);
            vs.push_back(v);
            es.push_back(i);
        }
    };

    dfs(dfs, s);

    std::reverse(us.begin(), us.end());
    std::reverse(vs.begin(), vs.end());
    std::reverse(es.begin(), es.end());

    for (i32 u = 0; u < n; ++u) if (deg[u]) return std::nullopt;
    for (i32 i = 1; i < static_cast<i32>(us.size()); ++i)
        if (vs[i - 1] != us[i]) return std::nullopt;

    us.push_back(vs.back());
    return std::make_pair(std::move(us), std::move(es));
}

#endif // LIB_EULERIAN_HPP
