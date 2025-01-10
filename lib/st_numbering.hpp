#ifndef LIB_ST_NUMBERING_HPP
#define LIB_ST_NUMBERING_HPP 1

#include <algorithm>
#include <cassert>
#include <optional>
#include <vector>

#include <lib/prelude.hpp>

template <typename Graph>
inline std::optional<std::vector<i32>> st_numbering(const Graph &g, i32 s, i32 t) {
    const i32 n = static_cast<i32>(g.size());

    if (n == 1) return std::vector<i32>{0};
    if (s == t) return std::nullopt;

    std::vector<i32> par(n, -1), tin(n, -1), low(n, -1);

    std::vector<i32> tour;
    tour.reserve(n);

    const auto dfs = [&](auto &&self, i32 u) -> void {
        tin[u] = static_cast<i32>(tour.size());
        tour.push_back(u);

        low[u] = u;
        for (const i32 v : g[u]) {
            if (u == v) continue;
            if (tin[v] == -1) {
                self(self, v);
                par[v] = u;
                if (tin[low[v]] < tin[low[u]]) low[u] = low[v];
            } else if (tin[v] < tin[low[u]]) {
                low[u] = v;
            }
        }
    };

    tin[s] = 0;
    tour.push_back(s);

    dfs(dfs, t);
    if (static_cast<i32>(tour.size()) != n) return std::nullopt;

    std::vector<i32> nxt(n, -1), prv(n);
    nxt[s] = t;
    prv[t] = s;

    std::vector<i32> sgn(n);
    sgn[s] = -1;

    for (i32 i = 2; i < n; ++i) {
        const i32 u = tour[i];
        const i32 p = par[u];

        if (sgn[low[u]] == -1) {
            const i32 q = prv[p];
            if (q == -1) return std::nullopt;

            nxt[q] = u;
            nxt[u] = p;

            prv[u] = q;
            prv[p] = u;

            sgn[p] = 1;
        } else {
            const i32 q = nxt[p];
            if (q == -1) return std::nullopt;

            nxt[p] = u;
            nxt[u] = q;

            prv[u] = p;
            prv[q] = u;

            sgn[p] = -1;
        }
    }

    std::vector<i32> a{s};
    while (a.back() != t) a.push_back(nxt[a.back()]);

    if (static_cast<i32>(a.size()) < n) return std::nullopt;
    assert(a[0] == s && a.back() == t);

    std::vector<i32> rnk(n, -1);
    for (i32 i = 0; i < n; ++i) rnk[a[i]] = i;

    assert(*std::min_element(rnk.begin(), rnk.end()) != -1);
    for (i32 i = 0; i < n; ++i) {
        bool l = false, r = false;
        const i32 u = a[i];

        for (const i32 v : g[u]) {
            if (rnk[v] < rnk[u]) l = true;
            if (rnk[u] < rnk[v]) r = true;
        }

        if (i > 0 && !l) return std::nullopt;
        if (i < n - 1 && !r) return std::nullopt;
    }

    return rnk;
}

#endif // LIB_ST_NUMBERING_HPP
