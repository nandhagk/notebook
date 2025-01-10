#ifndef LIB_DSU_POTENTIAL_HPP
#define LIB_DSU_POTENTIAL_HPP 1

#include <cassert>
#include <numeric>
#include <optional>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_group_t<Monoid> * = nullptr>
struct dsu_potential {
    using MX = Monoid;
    using X = typename MX::ValueT;

    i32 n, ccs;
    std::vector<X> z;
    std::vector<i32> par, sz;

    dsu_potential() {}
    explicit dsu_potential(i32 m) {
        build(m);
    }

    void build(i32 m) {
        n = ccs = m;
        z.assign(n, MX::unit());

        par.resize(n);
        std::iota(par.begin(), par.end(), 0);

        sz.assign(n, 1);
    }

    std::pair<i32, X> find(i32 u) {
        assert(0 <= u && u < n);

        X x = MX::unit();
        while (u != par[u]) {
            x = MX::op(z[u], x);
            x = MX::op(z[par[u]], x);

            z[u] = MX::op(z[par[u]], z[u]);
            u = par[u] = par[par[u]];
        }

        return {u, x};
    }

    std::optional<X> same(i32 u, i32 v) {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);

        const auto [ru, xu] = find(u);
        const auto [rv, xv] = find(v);

        if (ru != rv) return std::nullopt;
        return MX::op(MX::inv(xu), xv);
    }

    bool merge(i32 u, i32 v, X x) {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);

        auto [ru, xu] = find(u);
        auto [rv, xv] = find(v);

        if (ru == rv) return false;
        if (sz[ru] < sz[rv]) {
            std::swap(ru, rv);
            std::swap(xu, xv);
            x = MX::inv(x);
        }

        x = MX::op(xu, x);
        x = MX::op(x, MX::inv(xv));

        z[rv] = x;
        par[rv] = ru;
        sz[ru] += sz[rv];
        --ccs;

        return true;
    }
};

#endif // LIB_DSU_POTENTIAL_HPP
