#ifndef LIB_HOPCROFT_KARP_HPP
#define LIB_HOPCROFT_KARP_HPP 1

#include <vector>
#include <algorithm>
#include <numeric>
#include <cassert>

#include <lib/prelude.hpp>
#include <lib/random.hpp>
#include <lib/bipartite.hpp>

inline std::vector<i32> hopcroft_karp(const i32 n, const i32 m, std::vector<std::pair<i32, i32>> e) {
        std::vector<i32> g(e.size()), l(n, -1), r(m, -1), deg(n + 1);
        std::shuffle(e.begin(), e.end(), MT);

        for (const auto &[x, y] : e) ++deg[x];
        std::inclusive_scan(deg.cbegin(), deg.cend(), deg.begin());

        for (const auto &[x, y] : e) g[--deg[x]] = y;
        
        std::vector<i32> q(n);

        bool match = false;
        do {
                std::vector<i32> a(n, -1), p(n, -1);

                i32 t = 0;
                for (i32 i = 0; i < n; ++i) {
                        if (l[i] == -1) q[t++] = a[i] = p[i] = i;
                }

                match = false;
                for (i32 i = 0; i < t; ++i) {
                        i32 x = q[i];
                        if (l[a[x]] != -1) continue;

                        for (i32 j = deg[x]; j < deg[x + 1]; ++j) {
                                i32 y = g[j];
                                if (r[y] == -1) {
                                        while (y != -1) {
                                                r[y] = x;
                                                std::swap(l[x], y);
                                                x = p[x];
                                        }

                                        match = true;
                                        break;
                                }

                                if (p[r[y]] == -1) {
                                        q[t++] = y = r[y];
                                        p[y] = x;
                                        a[y] = a[x];
                                }
                        }
                }
        } while (match);

        return l;
}

inline std::vector<i32> hopcroft_karp(const std::vector<std::vector<i32>> &g, const std::vector<i32> &color) {
        const i32 n = static_cast<i32>(g.size());

        i32 m{};
        for (i32 u = 0; u < n; ++u) {
                m += static_cast<i32>(g[u].size());
        }

        m /= 2;

        std::vector<i32> vs(n);
        std::iota(vs.begin(), vs.end(), 0);

        const auto it = std::partition(vs.begin(), vs.end(), [&](const i32 u) { 
                return color[u] == 0; 
        });

        const i32 p = static_cast<i32>(it - vs.begin());
        const i32 q = n - p;

        std::vector<i32> rv(n);
        for (i32 i = 0; i < n; ++i) rv[vs[i]] = i;

        std::vector<std::pair<i32, i32>> es;
        es.reserve(m);

        for (i32 u = 0; u < n; ++u) {
                for (const i32 v : g[u]) {
                        if (color[u]) continue;
                        es.emplace_back(rv[u], rv[v] - p);
                }
        }

        const auto l = hopcroft_karp(p, q, std::move(es));

        std::vector<i32> mate(n, -1);
        for (i32 i = 0; i < p; ++i) {
                if (l[i] == -1) continue;

                mate[vs[i]] = vs[l[i] + p];
                mate[vs[l[i] + p]] = vs[i];
        }

        return mate;
}

inline std::vector<i32> hopcroft_karp(const std::vector<std::vector<i32>> &g) {
        const auto &[is_bipartite, color] = bipartition(g);
        assert(is_bipartite);

        return hopcroft_karp(g, color);
}

#endif // LIB_HOPCROFT_KARP_HPP
