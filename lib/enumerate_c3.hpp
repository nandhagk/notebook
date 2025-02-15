#ifndef LIB_ENUMERATE_C3_HPP
#define LIB_ENUMERATE_C3_HPP 1

#include <vector>

#include <lib/csr_graph.hpp>
#include <lib/prelude.hpp>

template <typename Graph, typename F>
inline void enumerate_c3(const Graph &g, F f) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<i32> deg(n);
    for (i32 u = 0; u < n; ++u) deg[u] = static_cast<i32>(g[u].size());

    std::vector<std::pair<i32, simple_edge>> es;
    es.reserve(std::accumulate(deg.begin(), deg.end(), 0));

    for (i32 u = 0; u < n; ++u) {
        for (const i32 v : g[u]) {
            if (u >= v) continue;

            if (deg[u] <= deg[v])
                es.emplace_back(v, u);
            else
                es.emplace_back(u, v);
        }
    }

    csr_graph h(n, es);

    std::vector<bool> x(n);
    for (i32 u = 0; u < n; ++u) {
        for (const i32 v : h[u]) x[v] = true;

        for (const i32 v : h[u]) {
            for (const i32 w : h[v])
                if (x[w]) f(u, v, w);
        }

        for (const i32 v : h[u]) x[v] = false;
    }
}

#endif // LIB_ENUMERATE_C3_HPP
