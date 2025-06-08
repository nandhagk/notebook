#ifndef LIB_DIJKSTRA_HPP
#define LIB_DIJKSTRA_HPP 1

#include <cassert>
#include <queue>
#include <vector>

#include <lib/graph_traits.hpp>
#include <lib/limits.hpp>
#include <lib/prelude.hpp>

template <typename Graph>
inline std::pair<std::vector<graph_weight_t<Graph>>, std::vector<i32>> dijkstra(const Graph &g, i32 s, i32 t = -1) {
    using W = graph_weight_t<Graph>;

    const i32 n = static_cast<i32>(g.size());

    assert(0 <= s && s < n);
    assert(-1 <= t && t < n);

    std::vector<W> dst(n, inf<W>);
    std::vector<i32> prv(n, -1);

    using Q = std::pair<W, i32>;
    std::priority_queue<Q, std::vector<Q>, std::greater<Q>> q;

    q.emplace(0, s);
    dst[s] = W(0);

    std::vector<bool> vis(n);
    while (!q.empty()) {
        const auto [_, u] = q.top();
        q.pop();

        if (u == t) break;

        if (vis[u]) continue;
        vis[u] = true;

        for (const auto &[v, w] : g[u]) {
            if (dst[u] + w < dst[v]) {
                dst[v] = dst[u] + w;
                prv[v] = u;
                q.emplace(dst[v], v);
            }
        }
    }

    return {std::move(dst), std::move(prv)};
}

#endif // LIB_DIJKSTRA_HPP 1
