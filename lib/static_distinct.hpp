#ifndef LIB_STATIC_DISTINCT_HPP
#define LIB_STATIC_DISTINCT_HPP 1

#include <vector>

#include <lib/compress.hpp>
#include <lib/prelude.hpp>
#include <lib/wavelet_matrix.hpp>

template <typename T>
struct static_distinct {
    i32 n;
    wavelet_matrix<i32> wm;

    static_distinct() {}
    explicit static_distinct(const std::vector<T> &v) {
        build(v);
    }

    void build(const std::vector<T> &v) {
        n = static_cast<i32>(v.size());
        const auto b = compress(v);

        std::vector<i32> last(n, -1), nxt(n, n);
        for (i32 i = 0; i < n; ++i) {
            if (last[b[i]] != -1) nxt[last[b[i]]] = i;
            last[b[i]] = i;
        }

        wm.build(nxt);
    }

    i32 prod(i32 l, i32 r) const {
        assert(0 <= l && l <= r && r <= n);

        return r - l - wm.count(l, r, r);
    }
};

#endif // LIB_STATIC_DISTINCT_HPP
