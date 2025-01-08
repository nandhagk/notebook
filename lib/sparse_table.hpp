#ifndef LIB_SPARSE_TABLE_HPP
#define LIB_SPARSE_TABLE_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_monoid_t<Monoid> * = nullptr>
struct sparse_table {
    using MX = Monoid;
    using X = typename MX::ValueT;

    i32 n, log;
    std::vector<std::vector<X>> d;

    sparse_table() {}
    explicit sparse_table(i32 m) { build(m); }

    explicit sparse_table(const std::vector<X> &v) { build(v); }

    template <typename F>
    sparse_table(i32 m, F f) {
        build(m, f);
    }

    void build(const std::vector<X> &v) {
        build(static_cast<i32>(v.size()), [&](i32 i) -> X { return v[i]; });
    }

    template <typename F>
    void build(i32 m, F f) {
        n = m;

        log = 1;
        while ((1 << log) < n) ++log;
        d.resize(log);

        d[0].resize(n);
        for (i32 i = 0; i < n; ++i) d[0][i] = f(i);

        for (i32 i = 0; i < log - 1; ++i) {
            const i32 k = static_cast<i32>(d[i].size()) - (1 << i);

            d[i + 1].resize(k);
            for (i32 j = 0; j < k; ++j) d[i + 1][j] = MX::op(d[i][j], d[i][j + (1 << i)]);
        }
    }

    X get(i32 p) const {
        assert(0 <= p && p < n);

        return prod(p, p + 1);
    }

    X prod(i32 l, i32 r) const {
        assert(0 <= l && l <= r && r <= n);

        if (l == r) return MX::unit();
        if (l + 1 == r) return d[0][l];

        const i32 k = topbit(r - l - 1);
        return MX::op(d[k][l], d[k][r - (1 << k)]);
    }
};

#endif // LIB_SPARSE_TABLE_HPP
