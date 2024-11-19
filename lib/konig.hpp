#ifndef LIB_KONIG_HPP
#define LIB_KONIG_HPP 1

#include <vector>
#include <queue>
#include <cassert>

#include <lib/prelude.hpp>
#include <lib/bipartite.hpp>
#include <lib/hopcroft_karp.hpp>

// UNTESTED !!!!!
inline std::vector<bool> konig(
        const std::vector<std::vector<i32>> &g, 
        const std::vector<i32> &color,
        const std::vector<i32> &mate) {
        const i32 n = static_cast<i32>(g.size());

        std::vector<bool> z(n, false);
        for (i32 u = 0; u < n; ++u) {
                if (mate[u] != -1 || color[u] == 1) continue;
                if (z[u]) continue;

                std::queue<i32> q;
                q.push(u);

                while (!q.empty()) {
                        const i32 v = q.front();
                        q.pop();

                        if (z[v]) continue;
                        z[v] = true;

                        if (color[v] == 1) {
                                if (mate[v] != -1) q.push(mate[v]);
                        } else {
                                for (const i32 w : g[v]) {
                                        if (mate[v] != w) q.push(w);
                                }
                        }
                }
        }

        std::vector<bool> cover(n, false);

        for (i32 u = 0; u < n; ++u) {
                const bool a = color[u] == 0 && !z[u];
                const bool b = color[u] == 1 && z[u];
                cover[u] = a || b;
        }

        for (i32 u = 0; u < n; ++u) {
                for (const i32 v : g[u]) {
                        assert(cover[u] || cover[v]);
                }
        }

        return cover;
}

inline std::vector<bool> konig(const std::vector<std::vector<i32>> &g) {
        const auto &[is_bipartite, color] = bipartition(g);
        assert(is_bipartite);

        const auto mate = hopcroft_karp(g, color);
        return konig(g, color, mate);
}

#endif // LIB_KONIG_HPP
