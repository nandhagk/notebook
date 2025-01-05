#ifndef LIB_STATIC_DISTINCT_HPP
#define LIB_STATIC_DISTINCT_HPP 1

#include <algorithm>
#include <numeric>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/wavelet_matrix.hpp>

template <typename T>
struct static_distinct {
    i32 n;
    wavelet_matrix<i32> wm;

    static_distinct() {}
    explicit static_distinct(const std::vector<T> &v) { build(v); }

    void build(const std::vector<T> &v) {
        n = static_cast<i32>(v.size());

        std::vector<i32> vi(n);
        std::iota(vi.begin(), vi.end(), 0);
        std::sort(vi.begin(), vi.end(), [&](const i32 i, const i32 j) { return v[i] == v[j] ? i < j : v[i] < v[j]; });

        std::vector<i32> rv;
        rv.reserve(n);

        std::vector<i32> b(n);
        for (const i32 i : vi) {
            if (rv.empty() || rv.back() != v[i]) rv.push_back(v[i]);
            b[i] = static_cast<i32>(rv.size()) - 1;
        }

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
