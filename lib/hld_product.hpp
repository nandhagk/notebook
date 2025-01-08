#ifndef LIB_HLD_PRODUCT_HPP
#define LIB_HLD_PRODUCT_HPP 1

#include <lib/hld.hpp>
#include <lib/monoids/reverse_monoid.hpp>
#include <lib/prelude.hpp>

template <template <typename> typename RangeProduct, typename Monoid>
constexpr auto extract(RangeProduct<Monoid> r) -> RangeProduct<monoid_reverse_monoid<Monoid>>;

template <typename RangeProduct>
struct hld_product {
    using MX = typename RangeProduct::MX;
    using X = typename MX::ValueT;

    const hld &h;

    RangeProduct st;
    decltype(extract(std::declval<RangeProduct>())) rst;

    explicit hld_product(const hld &g) : h(g) { build(); }

    hld_product(const hld &g, const std::vector<X> &v) : h(g) { build(v); }

    template <typename F>
    hld_product(const hld &g, F f) : h(g) { build(f); }

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

    X get(i32 u) const { return st.get(h.tin[u]); }

    void set(i32 u, X x) {
        st.set(h.tin[u], x);
        if constexpr (!MX::commutative) rst.set(h.tin[u], x);
    }

    void multiply(i32 u, X x) {
        st.multiply(h.tin[u], x);
        if constexpr (!MX::commutative) rst.multiply(h.tin[u], x);
    }

    X prod_all() const { return st.prod_all(); }

    X prod_path(i32 u, i32 v) const {
        X x = MX::unit();
        for (const auto &[s, t] : h.decompose(u, v)) x = MX::op(x, prod(s, t));

        return x;
    }

    X prod_subtree(i32 u) const {
        static_assert(MX::commutative);
        return st.prod(h.tin[u], h.tin[u] + h.sz[u]);
    }

private:
    X prod(i32 u, i32 v) const {
        const i32 a = h.tin[u];
        const i32 b = h.tin[v];

        if (a <= b) return st.prod(a, b + 1);
        if constexpr (!MX::commutative) return rst.prod(b, a + 1);

        return st.prod(b, a + 1);
    }
};

#endif // LIB_HLD_PRODUCT_HPP
