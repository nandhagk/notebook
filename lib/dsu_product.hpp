#ifndef LIB_DSU_PRODUCT_HPP
#define LIB_DSU_PRODUCT_HPP 1

#include <algorithm>
#include <cassert>
#include <vector>

#include <lib/algebraic_traits.hpp>
#include <lib/prelude.hpp>

template <typename Monoid, is_commutative_monoid_t<Monoid> * = nullptr>
struct dsu_product {
    using MX = Monoid;
    using X = typename MX::ValueT;

    i32 n, ccs;
    std::vector<i32> d;
    std::vector<X> x;

    dsu_product() {}
    explicit dsu_product(i32 m) {
        build(m);
    }

    void build(i32 m) {
        n = ccs = m;
        d.assign(n, -1);
        x.assign(n, MX::unit());
    }

    i32 find(i32 u) {
        assert(0 <= u && u < n);

        if (d[u] < 0) return u;
        return d[u] = find(d[u]);
    }

    bool merge(i32 u, i32 v) {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);

        u = find(u);
        v = find(v);

        if (u == v) return false;
        if (-d[u] < -d[v]) std::swap(u, v);

        d[u] += d[v];
        d[v] = u;
        x[u] = MX::op(x[u], x[v]);
        --ccs;

        return true;
    }

    bool same(i32 u, i32 v) {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);

        return find(u) == find(v);
    }

    void multiply(i32 u, X s) {
        assert(0 <= u && u < n);

        u = find(u);
        x[u] = MX::op(x[u], s);
    }

    X prod(i32 u) {
        assert(0 <= u && u < n);

        return x[find(u)];
    }

    i32 size(i32 u) {
        assert(0 <= u && u < n);

        return -d[find(u)];
    }
};

#endif // LIB_DSU_PRODUCT_HPP
