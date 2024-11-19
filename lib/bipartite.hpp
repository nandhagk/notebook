#ifndef LIB_BIPARTITE_HPP
#define LIB_BIPARTITE_HPP 1

#include <vector>
#include <queue>
#include <cassert>

#include <lib/prelude.hpp>

inline bool is_bipartite(const std::vector<std::vector<i32>> &g) {
        const i32 n = static_cast<i32>(g.size());

        std::queue<i32> q;
        std::vector<i32> color(n, -1);
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
                                        return false;
                                }
                        }
                }
        }

        return true;
}

// Will panic if not bipartite
inline std::vector<i32> bipartition(const std::vector<std::vector<i32>> &g) {
        const i32 n = static_cast<i32>(g.size());

        std::queue<i32> q;
        std::vector<i32> color(n, -1);
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
                                } else {
                                        assert(color[s] != color[v]);
                                }
                        }
                }
        }

        return color;
}

#endif // LIB_BIPARTITE_HPP
