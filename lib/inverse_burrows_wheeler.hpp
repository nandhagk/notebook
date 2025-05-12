#ifndef LIB_BURROWS_WHEELER_HPP
#define LIB_BURROWS_WHEELER_HPP 1

#include <algorithm>
#include <cassert>
#include <numeric>
#include <string>
#include <vector>

#include <lib/prelude.hpp>

inline std::vector<i32> inverse_burrows_wheeler(const std::vector<i32> &s, i32 z) {
    const i32 n = static_cast<i32>(s.size());

    const i32 a = *std::min_element(s.begin(), s.end());
    assert(a == z);

    const i32 b = *std::max_element(s.begin(), s.end());
    const i32 c = b - a + 2;

    std::vector<i32> t(c), nxt(n);

    for (const i32 x : s) ++t[x - a + 1];
    std::partial_sum(t.begin(), t.end(), t.begin());

    for (i32 i = 0; i < n; ++i) nxt[t[s[i] - a]++] = i;

    i32 cur = nxt[0];
    std::vector<i32> r(n - 1, z);

    for (i32 i = 0; i < n - 1; ++i) r[i] = s[cur = nxt[cur]];
    return r;
}

inline std::string inverse_burrows_wheeler(const std::string &s, char z) {
    const i32 n = static_cast<i32>(s.size());

    std::vector<i32> t(n);
    for (i32 i = 0; i < n; ++i) t[i] = s[i];

    const auto r = inverse_burrows_wheeler(t, z);

    std::string w(n - 1, z);
    for (i32 i = 0; i < n - 1; ++i) w[i] = static_cast<char>(r[i]);

    return w;
}

#endif // LIB_BURROWS_WHEELER_HPP
