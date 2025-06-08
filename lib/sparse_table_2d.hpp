#ifndef LIB_SPARSE_TABLE_2D_HPP
#define LIB_SPARSE_TABLE_2D_HPP 1

#include <cassert>
#include <vector>

#include <lib/algebraic_traits.hpp>
#include <lib/prelude.hpp>
#include <lib/sparse_table.hpp>

template <typename Monoid, is_commutative_monoid_t<Monoid> * = nullptr>
struct sparse_table_2d {
    using MX = Monoid;
    using X = typename MX::ValueT;

    i32 H, W;
    std::vector<std::vector<sparse_table<Monoid>>> d;

    sparse_table_2d() {}

    sparse_table_2d(i32 h, i32 w) {
        build(h, w);
    }

    explicit sparse_table_2d(const std::vector<std::vector<X>> &v) {
        build(v);
    }

    template <typename F>
    sparse_table_2d(i32 h, i32 w, F f) {
        build(h, w, f);
    }

    void build(const std::vector<std::vector<X>> &v) {
        const i32 h = static_cast<i32>(v.size());
        const i32 w = static_cast<i32>(v[0].size());

        build(h, w, [&](i32 i, i32 j) -> X { return v[i][j]; });
    }

    template <typename F>
    void build(i32 h, i32 w, F f) {
        H = h;
        W = w;

        i32 log = 1;
        while ((1 << log) < h) ++log;
        d.resize(log);

        d[0].resize(h);
        for (i32 i = 0; i < h; ++i) d[0][i].build(w, [&](i32 j) -> X { return f(i, j); });

        for (i32 i = 0; i < log - 1; ++i) {
            const i32 k = static_cast<i32>(d[i].size()) - (1 << i);

            d[i + 1].resize(k);
            for (i32 j = 0; j < k; ++j)
                d[i + 1][j].build(w, [&](i32 p) -> X { return MX::op(d[i][j].d[0][p], d[i][j + (1 << i)].d[0][p]); });
        }
    }

    X prod(i32 xl, i32 xr, i32 yl, i32 yr) const {
        assert(0 <= xl && xl <= xr && xr <= H);
        assert(0 <= yl && yl <= yr && yr <= W);

        if (xl == xr) return MX::unit();
        if (xl + 1 == xr) return d[0][xl].prod(yl, yr);

        const i32 k = topbit(xr - xl - 1);
        return MX::op(d[k][xl].prod(yl, yr), d[k][xr - (1 << k)].prod(yl, yr));
    }
};

#endif // LIB_SPARSE_TABLE_2D_HPP
