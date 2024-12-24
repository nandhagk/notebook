#ifndef LIB_HLD_FENWICK_TREE_HPP
#define LIB_HLD_FENWICK_TREE_HPP 1

#include <lib/fenwick_tree.hpp>
#include <lib/hld.hpp>
#include <lib/prelude.hpp>

template <typename Monoid> struct hld_fenwick_tree {
    using MX = Monoid;
    using X = typename MX::ValueT;

    const hld &h;

    fenwick_tree<MX> ft;

    explicit hld_fenwick_tree(const hld &g) : h(g) { build(); }

    template <typename F> hld_fenwick_tree(const hld &g, F f) : h(g) { build(f); }

    explicit hld_fenwick_tree(const hld &g, const std::vector<X> &v) : h(g) { build(v); }

    void build() {
        build([](i32) -> X { return MX::unit(); });
    }

    void build(const std::vector<X> &v) {
        build([&](i32 u) -> X { return v[h.tour[u]]; });
    }

    template <typename F> void build(F f) { ft.build(h.n, f); }

    X get(i32 u) { return ft.get(h.tin[u]); }

    void set(i32 u, X x) { ft.set(h.tin[u], x); }

    void multiply(i32 u, X x) { ft.multiply(h.tin[u], x); }

    X prod_path(i32 u, i32 v) {
        X x = MX::unit();
        for (const auto &[s, t] : h.decompose(u, v)) {
            const auto &[l, r] = std::minmax(h.tin[s], h.tin[t]);
            x = MX::op(x, ft.prod(l, r + 1));
        }

        return x;
    }

    X prod_subtree(i32 u) { return ft.prod(h.tin[u], h.tin[u] + h.sz[u]); }
};

#endif // LIB_HLD_FENWICK_TREE_HPP
