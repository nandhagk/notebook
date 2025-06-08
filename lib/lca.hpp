#ifndef LIB_LCA_HPP
#define LIB_LCA_HPP 1

#include <cassert>
#include <vector>

#include <lib/monoids/min.hpp>
#include <lib/prelude.hpp>
#include <lib/sparse_table.hpp>

struct lca {
    lca() {}

    template <typename Tree>
    lca(const Tree &g)
        : lca(g, 0) {}

    template <typename Tree>
    lca(const Tree &g, i32 root) {
        build(g, root);
    }

    template <typename Tree>
    void build(const Tree &g) {
        build(g, 0);
    }

    template <typename Tree>
    void build(const Tree &g, i32 root) {
        n = static_cast<i32>(g.size());

        s.assign(n, -1);

        p.clear();
        e.clear();

        p.reserve(2 * n);
        e.reserve(2 * n);

        dfs(g, root);
        sp.build(e);
    }

    i32 prod(i32 u, i32 v) const {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);

        const auto [l, r] = std::minmax(s[u], s[v]);
        return p[sp.prod(l, r + 1)];
    };

private:
    template <typename Tree>
    void dfs(const Tree &g, i32 u, i32 t = -1) {
        s[u] = static_cast<i32>(p.size());

        p.push_back(u);
        e.push_back(s[u]);

        for (const i32 v : g[u]) {
            if (v == t) continue;

            p.push_back(u);
            e.push_back(s[u]);

            dfs(g, v, u);
        }
    }

    i32 n;
    std::vector<i32> s, p, e;
    sparse_table<monoid_min<i32>> sp;
};

#endif // LIB_LCA_HPP
