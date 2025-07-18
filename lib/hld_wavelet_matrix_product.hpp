#ifndef LIB_HLD_WAVELET_MATRIX_PRODUCT_HPP
#define LIB_HLD_WAVELET_MATRIX_PRODUCT_HPP 1

#include <lib/hld.hpp>
#include <lib/prelude.hpp>
#include <lib/wavelet_matrix_product.hpp>

template <typename T, typename RangeProduct>
struct hld_wavelet_matrix_product {
    const hld &h;
    wavelet_matrix_product<T, RangeProduct> wm;

    using MX = typename RangeProduct::MX;
    using X = typename MX::ValueT;

    explicit hld_wavelet_matrix_product(const hld &g, const std::vector<T> &v, const std::vector<X> &s)
        : h(g) {
        build(v, s);
    }

    void build(const std::vector<T> &v, const std::vector<X> &s) {
        std::vector<T> a(h.n);
        for (i32 u = 0; u < h.n; ++u) a[u] = v[h.tour[u]];

        std::vector<X> b(h.n);
        for (i32 u = 0; u < h.n; ++u) b[u] = s[h.tour[u]];

        wm.build(a, b);
    }

    std::pair<i32, X> count_path(i32 u, i32 v, T a) const {
        i32 cnt{};
        X r = MX::unit();

        for (const auto &[s, t] : h.decompose(u, v)) {
            const auto &[x, y] = std::minmax(h.tin[s], h.tin[t]);
            const auto &[c, p] = wm.count(x, y + 1, a);

            cnt += c;
            r = MX::op(r, p);
        }

        return {cnt, r};
    }

    std::pair<i32, X> count_path(i32 u, i32 v, T a, T b) const {
        i32 cnt{};
        X r = MX::unit();

        for (const auto &[s, t] : h.decompose(u, v)) {
            const auto &[x, y] = std::minmax(h.tin[s], h.tin[t]);
            const auto &[c, p] = wm.count(x, y + 1, a, b);

            cnt += c;
            r = MX::op(r, p);
        }

        return {cnt, r};
    }

    std::pair<i32, X> count_subtree(i32 u, T a) const {
        return wm.count(h.tin[u], h.tin[u] + h.sz[u], a);
    }

    std::pair<i32, X> count_subtree(i32 u, T a, T b) const {
        return wm.count(h.tin[u], h.tin[u] + h.sz[u], a, b);
    }

    std::pair<T, X> kth_path(i32 u, i32 v, i32 k) const {
        assert(0 <= k && k <= h.dist(u, v));

        std::vector<std::pair<i32, i32>> segments;
        for (const auto &[s, t] : h.decompose(u, v)) {
            const auto &[x, y] = std::minmax(h.tin[s], h.tin[t]);
            segments.emplace_back(x, y + 1);
        }

        return wm.kth(std::move(segments), k);
    }

    std::pair<T, X> kth_subtree(i32 u, i32 k) const {
        return wm.kth(h.tin[u], h.tin[u] + h.sz[u], k);
    }

    void set(i32 u, const X &x) {
        wm.set(h.tin[u], x);
    }

    void multiply(i32 u, const X &x) {
        wm.mutiply(h.tin[u], x);
    }
};

#endif // LIB_HLD_WAVELET_MATRIX_PRODUCT_HPP
