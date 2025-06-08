#ifndef LIB_STATIC_PRODUCT_HPP
#define LIB_STATIC_PRODUCT_HPP 1

#include <cassert>
#include <vector>

#include <lib/algebraic_traits.hpp>
#include <lib/prelude.hpp>

template <typename Monoid, is_group_t<Monoid> * = nullptr>
struct static_product {
    using MX = Monoid;
    using X = typename MX::ValueT;

    i32 n;
    std::vector<X> d;

    static_product() {}
    explicit static_product(i32 m) {
        build(m);
    }

    explicit static_product(const std::vector<X> &v) {
        build(v);
    }

    template <typename F>
    static_product(i32 m, F f) {
        build(m, f);
    }

    void build(const std::vector<X> &v) {
        build(static_cast<i32>(v.size()), [&](i32 i) -> X { return v[i]; });
    }

    template <typename F>
    void build(i32 m, F f) {
        n = m;
        d.assign(n + 1, MX::unit());

        for (i32 i = 0; i < n; ++i) d[i + 1] = MX::op(d[i], f(i));
    }

    X get(i32 p) const {
        assert(0 <= p && p < n);

        return prod(p, p + 1);
    }

    X prod(i32 l, i32 r) const {
        assert(0 <= l && l <= r && r <= n);

        return MX::op(MX::inv(d[l]), d[r]);
    }

    X prod_all() const {
        return d[n];
    }
};

#endif // LIB_STATIC_PRODUCT_HPP
