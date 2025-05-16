#ifndef LIB_HLD_LAZY_SEGMENT_TREE_HPP
#define LIB_HLD_LAZY_SEGMENT_TREE_HPP 1

#include <lib/hld.hpp>
#include <lib/lazy_segment_tree.hpp>
#include <lib/monoids/reverse_monoid.hpp>
#include <lib/prelude.hpp>

template <typename ActedMonoid>
struct hld_lazy_segment_tree {
    using AM = ActedMonoid;

    using MX = typename AM::MX;
    using MA = typename AM::MA;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    const hld &h;

    struct RAM {
        using MX = monoid_reverse_monoid<hld_lazy_segment_tree::MX>;
        using MA = hld_lazy_segment_tree::MA;

        using X = typename MX::ValueT;
        using A = typename MA::ValueT;

        static constexpr X act(const X &x, const A &a, i32 sz) {
            return AM::act(x, a, sz);
        }
    };

    lazy_segment_tree<ActedMonoid> st;
    lazy_segment_tree<RAM> rst;

    explicit hld_lazy_segment_tree(const hld &g)
        : h(g) {
        build();
    }

    template <typename F>
    hld_lazy_segment_tree(const hld &g, F f)
        : h(g) {
        build(f);
    }

    explicit hld_lazy_segment_tree(const hld &g, const std::vector<X> &v)
        : h(g) {
        build(v);
    }

    void build() {
        build([](i32) -> X { return MX::unit(); });
    }

    void build(const std::vector<X> &v) {
        build([&](i32 u) -> X { return v[h.tour[u]]; });
    }

    template <typename F>
    void build(F f) {
        st.build(h.n, f);
        if constexpr (!MX::commutative) rst.build(h.n, f);
    }

    X get(i32 u) {
        return st.get(h.tin[u]);
    }

    void set(i32 u, const X &x) {
        st.set(h.tin[u], x);
        if constexpr (!MX::commutative) rst.set(h.tin[u], x);
    }

    void multiply(i32 u, const X &x) {
        st.multiply(h.tin[u], x);
        if constexpr (!MX::commutative) rst.multiply(h.tin[u], x);
    }

    X prod_path(i32 u, i32 v) {
        X x = MX::unit();
        for (const auto &[s, t] : h.decompose(u, v)) x = MX::op(x, prod(s, t));

        return x;
    }

    X prod_subtree(i32 u) {
        static_assert(MX::commutative);
        return st.prod(h.tin[u], h.tin[u] + h.sz[u]);
    }

    X prod_all() {
        return st.prod_all();
    }

    void apply_path(i32 u, i32 v, const A &a) {
        for (const auto &[s, t] : h.decompose(u, v)) {
            const auto &[x, y] = std::minmax(h.tin[s], h.tin[t]);

            st.apply(x, y + 1, a);
            if constexpr (!MX::commutative) rst.apply(x, y + 1, a);
        }
    }

    void apply_subtree(i32 u, const A &a) {
        const i32 x = h.tin[u];
        const i32 y = h.tin[u] + h.sz[u];

        st.apply(x, y, a);
        if constexpr (!MX::commutative) rst.apply(x, y, a);
    }

    void apply_outtree(i32 u, const A &a) {
        const i32 x = h.tin[u];
        const i32 y = h.tin[u] + h.sz[u];

        st.apply(0, x, a);
        st.apply(y, h.n, a);
        if constexpr (!MX::commutative) {
            rst.apply(0, x, a);
            rst.apply(y, h.n, a);
        }
    }

private:
    X prod(i32 u, i32 v) {
        const i32 a = h.tin[u];
        const i32 b = h.tin[v];

        if (a <= b) return st.prod(a, b + 1);
        if constexpr (!MX::commutative) return rst.prod(b, a + 1);

        return st.prod(b, a + 1);
    }
};

#endif // LIB_HLD_LAZY_SEGMENT_TREE_HPP
