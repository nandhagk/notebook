#ifndef LIB_TOPOLOGICAL_SORT_HPP
#define LIB_TOPOLOGICAL_SORT_HPP 1

#include <optional>
#include <queue>
#include <vector>

#include <lib/prelude.hpp>

template <typename DAG>
inline std::optional<std::vector<i32>> topological_sort(const DAG &g) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<i32> in(n);
    for (i32 u = 0; u < n; ++u)
        for (const i32 v : g[u]) ++in[v];

    std::queue<i32> q;
    for (i32 u = 0; u < n; ++u)
        if (in[u] == 0) q.push(u);

    std::vector<i32> ord;
    ord.reserve(n);

    while (!q.empty()) {
        const i32 u = q.front();
        q.pop();

        ord.push_back(u);
        for (const i32 v : g[u])
            if (--in[v] == 0) q.push(v);
    }

    if (static_cast<i32>(ord.size()) != n) return std::nullopt;
    return ord;
}

#endif // LIB_TOPOLOGICAL_SORT_HPP
