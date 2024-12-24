#ifndef LIB_BLOSSOMS_HPP
#define LIB_BLOSSOMS_HPP 1

#include <algorithm>
#include <numeric>
#include <queue>
#include <vector>

#include <lib/prelude.hpp>

template <typename Graph> inline std::vector<i32> blossoms(const Graph &g) {
    const i32 n = static_cast<i32>(g.size());

    i32 time{};
    std::vector<i32> mate(n, -1), orig(n), par(n, -1), aux(n);

    const auto augment = [&](i32 u) -> void {
        while (u != -1) {
            const i32 v = mate[par[u]];
            mate[mate[u] = par[u]] = u;
            u = v;
        }
    };

    const auto lca = [&](i32 u, i32 v) -> i32 {
        ++time;
        for (;;) {
            if (u == -1) std::swap(u, v);
            if (aux[u] == time) return u;

            aux[u] = time;
            u = mate[u] == -1 ? -1 : orig[par[mate[u]]];
        }

        return 0;
    };

    for (i32 s = 0; s < n; ++s) {
        if (mate[s] != -1) continue;

        std::vector<i32> label(n, -1);
        std::iota(orig.begin(), orig.end(), 0);

        std::queue<i32> q;

        label[s] = 0;
        q.push(s);

        const auto blossom = [&](i32 u, i32 v, i32 w) -> void {
            while (orig[u] != w) {
                par[u] = v;
                v = mate[u];

                if (label[v] == 1) {
                    label[v] = 0;
                    q.push(v);
                }

                orig[u] = orig[v] = w;
                u = par[v];
            }
        };

        while (!q.empty()) {
            const i32 u = q.front();
            q.pop();

            for (const i32 v : g[u]) {
                if (label[v] == -1) {
                    par[v] = u;
                    label[v] = 1;
                    if (mate[v] == -1) {
                        augment(v);
                        goto done;
                    }

                    q.push(mate[v]);
                    label[mate[v]] = 0;
                } else if (!label[v] && orig[u] != orig[v]) {
                    const i32 w = lca(u, v);

                    blossom(u, v, w);
                    blossom(v, u, w);

                    for (i32 t = 0; t < n; ++t) orig[t] = orig[orig[t]];
                }
            }
        }

    done:;
    }

    return mate;
}

#endif // LIB_BLOSSOMS_HPP
