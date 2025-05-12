#ifndef LIB_INVERSE_SUFFIX_ARRAY_HPP
#define LIB_INVERSE_SUFFIX_ARRAY_HPP 1

#include <vector>

#include <lib/prelude.hpp>

inline std::vector<i32> inverse_suffix_array(const std::vector<i32> &sa) {
    const i32 n = static_cast<i32>(sa.size());

    std::vector<i32> rnk(n);
    for (i32 i = 0; i < n; ++i) rnk[sa[i]] = i;

    std::vector<i32> s(n);
    for (i32 k = 1; k < n; ++k) {
        const i32 i = sa[k - 1];
        const i32 j = sa[k];
        s[j] = s[i];
        if (i < n - 1 && (j == n - 1 || (rnk[i + 1] > rnk[j + 1]))) ++s[j];
    }

    return s;
}

#endif // LIB_INVERSE_SUFFIX_ARRAY_HPP
