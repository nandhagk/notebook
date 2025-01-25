#ifndef LIB_LCP_ARRAY_HPP
#define LIB_LCP_ARRAY_HPP 1

#include <cassert>
#include <string>
#include <vector>

#include <lib/prelude.hpp>

// Reference:
// T. Kasai, G. Lee, H. Arimura, S. Arikawa, and K. Park,
// Linear-Time Longest-Common-Prefix Computation in Suffix
// Arrays and Its Applications
template <typename T>
std::vector<i32> lcp_array(const std::vector<T> &s, const std::vector<i32> &sa) {
    const i32 n = static_cast<i32>(s.size());
    assert(n >= 1);

    std::vector<i32> rnk(n);
    for (i32 i = 0; i < n; ++i) rnk[sa[i]] = i;

    std::vector<i32> lcp(n - 1);

    i32 h = 0;
    for (i32 i = 0; i < n; ++i) {
        if (h > 0) h--;
        if (rnk[i] == 0) continue;

        const i32 j = sa[rnk[i] - 1];
        for (; j + h < n && i + h < n; h++)
            if (s[j + h] != s[i + h]) break;

        lcp[rnk[i] - 1] = h;
    }

    return lcp;
}

inline std::vector<i32> lcp_array(const std::string &s, const std::vector<i32> &sa) {
    const i32 n = static_cast<i32>(s.size());

    std::vector<i32> t(n);
    for (i32 i = 0; i < n; ++i) t[i] = s[i];

    return lcp_array(t, sa);
}

#endif // LIB_LCP_ARRAY_HPP
