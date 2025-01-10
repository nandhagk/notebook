#ifndef LIB_ROLLBACK_DSU_PRODUCT_HPP
#define LIB_ROLLBACK_DSU_PRODUCT_HPP 1

#include <algorithm>
#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_abelian_group_t<Monoid> * = nullptr>
struct rollback_dsu_product {
    using MX = Monoid;
    using X = typename MX::ValueT;

    i32 n, ccs;
    std::vector<i32> d;
    std::vector<X> x;
    std::vector<std::pair<i32, i32>> h;

    rollback_dsu_product() {}
    explicit rollback_dsu_product(i32 m) {
        build(m);
    }

    void build(i32 m) {
        n = ccs = m;
        d.assign(n, -1);
        x.assign(n, MX::unit());

        h.clear();
        h.reserve(n);
    }

    i32 find(i32 u) const {
        assert(0 <= u && u < n);

        return d[u] < 0 ? u : find(d[u]);
    }

    void multiply(i32 u, X s) {
        assert(0 <= u && u < n);

        while (u >= 0) {
            x[u] = MX::op(x[u], s);
            u = d[u];
        }
    }

    bool merge(i32 u, i32 v) {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);

        u = find(u);
        v = find(v);

        if (u == v) return false;
        if (-d[u] < -d[v]) std::swap(u, v);

        h.emplace_back(v, d[v]);
        d[u] += d[v];
        x[u] = MX::op(x[u], x[v]);
        d[v] = u;
        --ccs;

        return true;
    }

    bool same(i32 u, i32 v) const {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);

        return find(u) == find(v);
    }

    i32 size(i32 u) const {
        assert(0 <= u && u < n);

        return -d[find(u)];
    }

    std::pair<i32, std::vector<i32>> ids() const {
        std::vector<std::vector<i32>> cc(n);
        for (i32 u = 0; u < n; ++u) cc[find(u)].push_back(u);

        std::vector<i32> ids;

        i32 group{};
        for (i32 u = 0; u < n; ++u) {
            if (cc[u].empty()) continue;

            for (const i32 v : cc[u]) ids[v] = group;
            ++group;
        }

        return {group, std::move(ids)};
    }

    X prod(i32 u) const {
        return x[find(u)];
    }

    void rollback() {
        assert(!h.empty());

        const auto [v, val] = h.back();
        const i32 u = d[v];

        d[v] = val;
        d[u] -= val;
        x[u] = MX::op(MX::inv(x[v]), x[u]);

        ++ccs;
        h.pop_back();
    }

    void rollback(i32 k) {
        assert(0 <= k);

        while (k--) rollback();
    }
};

#endif // LIB_ROLLBACK_DSU_PRODUCT_HPP
