#ifndef LIB_BIPARTITE_HPP
#define LIB_BIPARTITE_HPP 1

#include <vector>

#include <lib/dsu.hpp>
#include <lib/prelude.hpp>

inline std::vector<i32> bipartition(const std::vector<std::vector<i32>> &g) {
        const i32 n = static_cast<i32>(g.size());

        dsu dsu(2 * n);
        for (i32 u = 0; u < n; ++u) {
                for (const i32 v : g[u]) dsu.merge(u, v + n);
        }

        for (i32 u = 0; u < n; ++u) {
                if (dsu.find(u) == dsu.find(u + n)) return {};
        }

        std::vector<i32> color(2 * n, -1);
        for (i32 u = 0; u < n; ++u) {
                if (dsu.find(u) == u && color[u] == -1) {
                        color[u] = 0;
                        color[dsu.find(u + n)] = 1;
                }
        }

        for (i32 u = 0; u < n; ++u) color[u] = color[dsu.find(u)];
        color.resize(n);
        
        return color;
}

#endif // LIB_BIPARTITE_HPP
