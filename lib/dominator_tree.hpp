#ifndef LIB_DOMINATOR_TREE_HPP
#define LIB_DOMINATOR_TREE_HPP 1

#include <numeric>
#include <vector>

#include <lib/prelude.hpp>

template <typename Graph>
inline std::vector<i32> dominator_tree(const Graph &g, const i32 root) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<i32> par(n, -1), tin(n, -1), seen;
    seen.reserve(n);

    const auto dfs = [&](auto &&self, i32 u) -> void {
        tin[u] = static_cast<i32>(seen.size());
        seen.push_back(u);

        for (const i32 v : g[u]) {
            if (tin[v] != -1) continue;

            par[v] = u;
            self(self, v);
        }
    };

    dfs(dfs, root);
    const i32 k = static_cast<i32>(seen.size());

    const auto h = g.reverse();

    std::vector<i32> sdom(n), mi(n), anc(n, -1), us(n);
    std::iota(mi.begin(), mi.end(), 0);
    std::iota(sdom.begin(), sdom.end(), 0);

    const auto find = [&](auto &&self, i32 u) -> i32 {
        const i32 p = anc[u];
        if (p == -1) return u;

        const i32 v = self(self, p);
        if (tin[sdom[mi[p]]] < tin[sdom[mi[u]]]) mi[u] = mi[p];

        return anc[u] = v;
    };

    std::vector<std::vector<i32>> bucket(n);
    for (i32 i = k - 1; i >= 1; --i) {
        const i32 w = seen[i];
        for (const i32 v : h[w]) {
            if (tin[v] == -1) continue;

            find(find, v);
            if (tin[sdom[mi[v]]] < tin[sdom[w]]) sdom[w] = sdom[mi[v]];
        }

        bucket[sdom[w]].push_back(w);

        for (const i32 v : bucket[par[w]]) {
            find(find, v);
            us[v] = mi[v];
        }

        bucket[par[w]].clear();
        anc[w] = par[w];
    }

    std::vector<i32> idom(n, -1);
    for (i32 i = 1; i < k; ++i) {
        const i32 w = seen[i];
        idom[w] = sdom[w] == sdom[us[w]] ? sdom[w] : idom[us[w]];
    }

    return idom;
}

#endif // LIB_DOMINATOR_TREE_HPP
