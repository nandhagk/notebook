#ifndef LIB_STATIC_MEX_HPP
#define LIB_STATIC_MEX_HPP 1

#include <lib/monoids/min.hpp>
#include <lib/rect_wavelet_matrix_product.hpp>
#include <lib/sparse_table.hpp>

// Very slow
struct static_mex {
    i32 n;
    rect_wavelet_matrix_product<i32, sparse_table<monoid_min<i32>>> rwm;

    static_mex() {}

    explicit static_mex(const std::vector<i32> &v) {
        build(v);
    }

    void build(const std::vector<i32> &v) {
        n = static_cast<i32>(v.size());
        assert(*std::min_element(v.begin(), v.end()) >= 0);

        std::vector<std::pair<i32, i32>> ps;
        std::vector<i32> ws;

        ps.reserve(2 * n + 1);
        ws.reserve(2 * n + 1);

        std::vector<i32> last(n + 1, -1);
        for (i32 i = 0; i < n; ++i) {
            if (v[i] >= n) continue;

            ps.emplace_back(i, last[v[i]]);
            ws.push_back(v[i]);

            last[v[i]] = i;
        }

        for (i32 i = 0; i <= n; ++i) {
            ps.emplace_back(n, last[i]);
            ws.push_back(i);
        }

        rwm.build(ps, ws);
    }

    i32 prod(i32 l, i32 r) const {
        assert(0 <= l && l <= r && r <= n);

        return rwm.prod(r, n + 1, -1, l);
    }
};

#endif // LIB_STATIC_MEX_HPP
