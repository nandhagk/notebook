#ifndef LIB_RECT_WAVELET_MATRIX_PRODUCT_HPP
#define LIB_RECT_WAVELET_MATRIX_PRODUCT_HPP 1

#include <algorithm>
#include <numeric>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/wavelet_matrix_product.hpp>

template <typename T, typename RangeProduct>
struct rect_wavelet_matrix_product {
    using MX = typename RangeProduct::MX;
    using X = typename MX::ValueT;

    using pt = std::pair<T, T>;

    std::vector<pt> pts;
    wavelet_matrix_product<T, RangeProduct> wm;

    rect_wavelet_matrix_product() {}
    explicit rect_wavelet_matrix_product(const std::vector<pt> &ps, const std::vector<X> &ws) {
        build(ps, ws);
    }

    void build(const std::vector<pt> &ps, const std::vector<X> &ws) {
        const i32 p = static_cast<i32>(ps.size());

        std::vector<i32> pi(p);
        std::iota(pi.begin(), pi.end(), 0);
        std::sort(pi.begin(), pi.end(), [&](i32 i, i32 j) { return ps[i] < ps[j]; });

        pts.resize(p);
        std::vector<T> ys(p);
        std::vector<X> zs(p);

        for (i32 i = 0; i < p; ++i) {
            pts[i] = ps[pi[i]];
            ys[i] = ps[pi[i]].second;
            zs[i] = ws[pi[i]];
        }

        wm.build(ys, std::move(zs));
    }

    X prod(T xl, T xr, T yl, T yr) const {
        const i32 l = id(xl, yl);
        const i32 r = id(xr, yl);

        return wm.count(l, r, yl, yr).second;
    }

    X get(T x, T y) const {
        return prod(x, x + 1, y, y + 1);
    }

    void multiply(T x, T y, const X &v) {
        const i32 p = id(x, y);
        wm.multiply(p, v);
    }

    void set(T x, T y, const X &v) {
        const i32 p = id(x, y);
        wm.set(p, v);
    }

private:
    inline i32 id(T x, T y) const {
        return static_cast<i32>(std::lower_bound(pts.begin(), pts.end(), pt{x, y}) - pts.begin());
    }
};

#endif // LIB_RECT_WAVELET_MATRIX_PRODUCT_HPP
