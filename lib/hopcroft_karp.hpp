#ifndef LIB_HOPCROFT_KARP_HPP
#define LIB_HOPCROFT_KARP_HPP 1

#include <algorithm>
#include <cassert>
#include <numeric>
#include <queue>
#include <vector>

#include <lib/prelude.hpp>

// Shuffle edges before invoking.
template <typename Graph> inline std::vector<i32> hopcroft_karp(const Graph &g, const std::vector<i32> &color) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<i32> vs(n);
    std::iota(vs.begin(), vs.end(), 0);

    std::vector<i32> rv(n);
    for (i32 i = 0; i < n; ++i) rv[vs[i]] = i;

    const auto it = std::partition(vs.begin(), vs.end(), [&](i32 u) { return color[u] == 0; });

    const i32 x = static_cast<i32>(it - vs.begin());
    std::vector<i32> l(x, -1), r(n - x, -1);

    bool match = false;
    std::queue<i32> q;
    do {
        std::vector<i32> a(x, -1), p(x, -1);

        for (i32 i = 0; i < x; ++i)
            if (l[i] == -1) q.push(a[i] = p[i] = i);

        match = false;
        while (!q.empty()) {
            i32 u = q.front();
            q.pop();

            if (l[a[u]] != -1) continue;

            for (const i32 w : g[rv[u]]) {
                i32 v = rv[w] - x;
                if (r[v] == -1) {
                    while (v != -1) {
                        r[v] = u;
                        std::swap(l[u], v);
                        u = p[u];
                    }

                    match = true;
                    break;
                }

                if (p[r[v]] == -1) {
                    q.push(v = r[v]);
                    p[v] = u;
                    a[v] = a[u];
                }
            }
        }
    } while (match);

    std::vector<i32> mate(n, -1);
    for (i32 i = 0; i < x; ++i) {
        if (l[i] == -1) continue;

        mate[vs[i]] = vs[l[i] + x];
        mate[vs[l[i] + x]] = vs[i];
    }

    return mate;
}

#endif // LIB_HOPCROFT_KARP_HPP
