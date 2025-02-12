#ifndef LIB_ERDOS_GALLAI_HPP
#define LIB_ERDOS_GALLAI_HPP 1

#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>

#include <lib/prelude.hpp>

// http://compalg.inf.elte.hu/~tony/Kutatas/EGHH/Comb-IvanyiLucz-23Nov.pdf
inline bool erdos_gallai(std::vector<i32> deg) {
    const i32 n = static_cast<i32>(deg.size());

    if (n == 0) return true;
    if (*std::max_element(deg.begin(), deg.end()) >= n) return false;
    if (std::reduce(deg.begin(), deg.end()) % 2) return false;

    std::vector<i32> cnt(n);
    for (i32 i = 0; i < n; ++i) ++cnt[deg[i]];

    i32 p = n;
    for (i32 i = 0; i < n; ++i)
        for (i32 j = 0; j < cnt[i]; ++j) deg[--p] = i;

    std::vector<i64> h(n);
    std::partial_sum(deg.cbegin(), deg.cend(), h.begin());

    i32 w = n - 1;
    for (i32 i = 0; i < n; ++i) {
        while (w >= 0 && deg[w] < i) --w;

        const i64 y = std::max(i, w);
        if (h[i] > y * (i + 1) + h[n - 1] - h[y]) return false;
    }

    return true;
}

#endif // LIB_ERDOS_GALLAI_HPP
