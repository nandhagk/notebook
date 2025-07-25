#ifndef LIB_BIPARTITION_HPP
#define LIB_BIPARTITION_HPP 1

#include <optional>
#include <queue>
#include <vector>

#include <lib/prelude.hpp>

template <typename BipartiteGraph>
inline std::optional<std::vector<i32>> bipartition(const BipartiteGraph &g) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<i32> color(n, -1);

    std::queue<i32> q;
    for (i32 u = 0; u < n; ++u) {
        if (color[u] != -1) continue;

        q.push(u);
        color[u] = 0;

        while (!q.empty()) {
            const i32 v = q.front();
            q.pop();

            for (const i32 s : g[v]) {
                if (color[s] == -1) {
                    color[s] = color[v] ^ 1;
                    q.push(s);
                } else if (color[s] == color[v]) {
                    return std::nullopt;
                }
            }
        }
    }

    return color;
}

#endif // LIB_BIPARTITION_HPP
