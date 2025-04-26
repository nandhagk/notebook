#ifndef LIB_CTD_HPP
#define LIB_CTD_HPP 1

#include <algorithm>
#include <vector>

#include <lib/prelude.hpp>

struct ctd {
    i32 n, root;
    std::vector<i32> sz, par, depth;

    ctd() {}

    template <typename Graph>
    explicit ctd(const Graph &g) {
        build(g);
    }

    template <typename Graph>
    void build(const Graph &g) {
        n = static_cast<i32>(g.size());

        sz.assign(n, 0);
        par.assign(n, -1);
        depth.assign(n, n);

        dfs(g, 0, -1, n, 0);
        root = static_cast<i32>(std::find(par.begin(), par.end(), -1) - par.begin());
    }

private:
    template <typename Graph>
    std::pair<i32, i32> dfs(const Graph &g, i32 u, i32 t, const i32 m, const i32 rnk) {
        i32 size = 1;
        for (const i32 v : g[u]) {
            if (v == t || depth[v] < rnk) continue;

            const auto &[z, c] = dfs(g, v, u, m, rnk);
            if (z == -1) return {-1, c};

            sz[v] = z;
            size += z;
        }

        if (size * 2 >= m) {
            sz[u] = m;
            depth[u] = rnk;

            for (const i32 v : g[u]) {
                if (v == t || depth[v] < rnk) continue;

                const auto &[z, c] = dfs(g, v, -1, sz[v], rnk + 1);
                par[c] = u;
            }

            if (t != -1) {
                const auto &[z, c] = dfs(g, t, -1, m - size, rnk + 1);
                par[c] = u;
            }

            return {-1, u};
        }

        return {size, -1};
    }
};

#endif // LIB_CTD_HPP
