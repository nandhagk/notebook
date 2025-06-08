#ifndef LIB_SEGMENT_TREE_2D_HPP
#define LIB_SEGMENT_TREE_2D_HPP 1

#include <cassert>
#include <vector>

#include <lib/algebraic_traits.hpp>
#include <lib/prelude.hpp>

template <typename Monoid, is_commutative_monoid_t<Monoid> * = nullptr>
struct segment_tree_2d {
    using MX = Monoid;
    using X = typename MX::ValueT;

    i32 H, W;
    std::vector<X> d;

    segment_tree_2d() {}
    segment_tree_2d(i32 h, i32 w) {
        build(h, w);
    }

    explicit segment_tree_2d(const std::vector<std::vector<X>> &v) {
        build(v);
    }

    template <typename F>
    segment_tree_2d(i32 h, i32 w, F f) {
        build(h, w, f);
    }

    void build(i32 h, i32 w) {
        build(h, w, [](i32, i32) -> X { return MX::unit(); });
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

        d.assign(4 * H * W, MX::unit());
        for (i32 i = 0; i < H; ++i)
            for (i32 j = 0; j < W; ++j) d[idx(H + i, W + j)] = f(i, j);

        for (i32 j = W; j < 2 * W; ++j)
            for (i32 i = H - 1; i >= 0; --i) d[idx(i, j)] = MX::op(d[idx((i << 1), j)], d[idx((i << 1) + 1, j)]);

        for (i32 i = 0; i < 2 * H; ++i)
            for (i32 j = W - 1; j >= 0; --j) d[idx(i, j)] = MX::op(d[idx(i, (j << 1))], d[idx(i, (j << 1) + 1)]);
    }

    X get(i32 p, i32 q) const {
        assert(0 <= p && p < H);
        assert(0 <= q && q < W);

        p += H;
        q += W;

        return d[idx(p, q)];
    }

    X prod(i32 xl, i32 xr, i32 yl, i32 yr) const {
        assert(0 <= xl && xl <= xr && xr <= H);
        assert(0 <= yl && yl <= yr && yr <= W);

        X r = MX::unit();

        for (xl += H, xr += H; xl < xr; xl >>= 1, xr >>= 1) {
            if (xl & 1) r = MX::op(r, prod(xl++, yl, yr));
            if (xr & 1) r = MX::op(prod(--xr, yl, yr), r);
        }

        return r;
    }

    void set(i32 p, i32 q, X x) {
        assert(0 <= p && p < H);
        assert(0 <= q && q < W);

        p += H;
        q += W;

        d[idx(p, q)] = x;

        i32 i = p;
        while (i >>= 1) d[idx(i, q)] = MX::op(d[idx((i << 1), q)], d[idx((i << 1) + 1, q)]);

        i = p;
        while (i) {
            i32 j = q;
            while (j >>= 1) d[idx(i, j)] = MX::op(d[idx(i, (j << 1))], d[idx(i, (j << 1) + 1)]);

            i >>= 1;
        }
    }

    void multiply(i32 p, i32 q, X x) {
        set(p, q, MX::op(get(p, q), x));
    }

private:
    inline i32 idx(i32 i, i32 j) const {
        return i * 2 * W + j;
    }

    X prod(i32 x, i32 yl, i32 yr) const {
        X r = MX::unit();

        for (yl += W, yr += W; yl < yr; yl >>= 1, yr >>= 1) {
            if (yl & 1) r = MX::op(r, d[idx(x, yl++)]);
            if (yr & 1) r = MX::op(d[idx(x, --yr)], r);
        }

        return r;
    }
};

#endif // LIB_SEGMENT_TREE_2D_HPP
