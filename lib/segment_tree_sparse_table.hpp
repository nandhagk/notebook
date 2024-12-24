#ifndef LIB_SEGMENT_TREE_SPARSE_TABLE_HPP
#define LIB_SEGMENT_TREE_SPARSE_TABLE_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename SparseTable> struct segment_tree_sparse_table {
    using MX = typename SparseTable::MX;
    using X = typename MX::ValueT;

    i32 H, W;
    std::vector<SparseTable> d;

    segment_tree_sparse_table() {}

    segment_tree_sparse_table(i32 h, i32 w) { build(h, w); }

    explicit segment_tree_sparse_table(const std::vector<std::vector<X>> &v) { build(v); }

    template <typename F> segment_tree_sparse_table(i32 h, i32 w, F f) { build(h, w, f); }

    void build(i32 h, i32 w) {
        build(h, w, [](i32, i32) -> X { return MX::unit(); });
    }

    void build(const std::vector<std::vector<X>> &v) {
        const i32 h = static_cast<i32>(v.size());
        const i32 w = static_cast<i32>(v[0].size());

        build(h, w, [&](i32 i, i32 j) -> X { return v[i][j]; });
    }

    template <typename F> void build(i32 h, i32 w, F f) {
        H = h;
        W = w;

        d.resize(2 * h);
        for (i32 i = 0; i < h; ++i) d[h + i].build(w, [&](i32 j) -> X { return f(i, j); });
        for (i32 i = h - 1; i >= 1; --i)
            d[i].build(w, [&](i32 j) -> X { return MX::op(d[(i << 1)].d[0][j], d[(i << 1) | 1].d[0][j]); });
    }

    X prod(i32 xl, i32 xr, i32 yl, i32 yr) const {
        assert(0 <= xl && xl <= xr && xr <= H);
        assert(0 <= yl && yl <= yr && yr <= W);

        X r = MX::unit();

        xl += H;
        xr += H;

        while (xl < xr) {
            if (xl & 1) r = MX::op(r, d[xl++].prod(yl, yr));
            if (xr & 1) r = MX::op(d[--xr].prod(yl, yr), r);

            xl >>= 1;
            xr >>= 1;
        }

        return r;
    }
};

#endif // LIB_SEGMENT_TREE_SPARSE_TABLE_HPP
