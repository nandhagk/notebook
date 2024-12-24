#ifndef LIB_KMP_HPP
#define LIB_KMP_HPP

#include <string>
#include <vector>

#include <lib/prelude.hpp>

template <typename T> std::vector<i32> kmp(const std::vector<T> &s) {
    const i32 n = static_cast<i32>(s.size());

    std::vector<i32> k(n);
    for (i32 i = 1; i < n; ++i) {
        i32 j = k[i - 1];
        while (j > 0 && s[i] != s[j]) j = k[j - 1];

        if (s[i] == s[j]) ++j;
        k[i] = j;
    }

    return k;
}

inline std::vector<i32> kmp(const std::string &s) {
    const i32 n = static_cast<i32>(s.size());

    std::vector<i32> t(n);
    for (i32 i = 0; i < n; ++i) t[i] = s[i];

    return kmp(t);
}

#endif // LIB_KMP_HPP
