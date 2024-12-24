#ifndef LIB_STATIC_PRODUCT_2D_HPP
#define LIB_STATIC_PRODUCT_2D_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_abelian_group_t<Monoid> * = nullptr> struct static_product_2d {
    using MX = Monoid;
    using X = typename MX::ValueT;

    i32 H, W;
    std::vector<X> d;

    static_product_2d(i32 h, i32 w) { build(h, w); }

    explicit static_product_2d(const std::vector<std::vector<X>> &v) { build(v); }

    template <typename F> static_product_2d(i32 h, i32 w, F f) { build(h, w, f); }

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

        d.assign((H + 1) * (W + 1), MX::unit());
        for (i32 i = 1; i <= H; ++i) {
            for (i32 j = 1; j <= W; ++j) {
                const i32 k = idx(i, j);
                d[k] = f(i - 1, j - 1);
                d[k] = MX::op(d[k], d[idx(i - 1, j)]);
                d[k] = MX::op(d[k], d[idx(i, j - 1)]);
                d[k] = MX::op(MX::inv(d[idx(i - 1, j - 1)]), d[k]);
            }
        }
    }

    X prod(i32 xl, i32 xr, i32 yl, i32 yr) const {
        assert(0 <= xl && xl <= xr && xr <= H);
        assert(0 <= yl && yl <= yr && yr <= W);

        X x = d[idx(xr, yr)];
        x = MX::op(MX::inv(d[idx(xl, yr)]), x);
        x = MX::op(MX::inv(d[idx(xr, yl)]), x);
        x = MX::op(x, d[idx(xl, yl)]);

        return x;
    }

  private:
    inline i32 idx(i32 x, i32 y) const { return (W + 1) * x + y; }
};

#endif // LIB_STATIC_PRODUCT_2D_HPP
