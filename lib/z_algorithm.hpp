#ifndef LIB_Z_ALGORITHM_HPP
#define LIB_Z_ALGORITHM_HPP

#include <algorithm>
#include <string>
#include <vector>

#include <lib/prelude.hpp>

template <typename T>
std::vector<i32> z_algorithm(const std::vector<T> &s) {
    const i32 n = static_cast<i32>(s.size());
    if (n == 0) return {};

    std::vector<i32> z(n);
    z[0] = 0;
    for (i32 i = 1, j = 0; i < n; ++i) {
        i32 &k = z[i];
        k = (j + z[j] <= i) ? 0 : std::min(j + z[j] - i, z[i - j]);

        while (i + k < n && s[k] == s[i + k]) ++k;
        if (j + z[j] < i + z[i]) j = i;
    }

    z[0] = n;
    return z;
}

inline std::vector<i32> z_algorithm(const std::string &s) {
    const i32 n = static_cast<i32>(s.size());

    std::vector<i32> t(n);
    for (i32 i = 0; i < n; ++i) t[i] = s[i];

    return z_algorithm(t);
}

#endif // LIB_Z_ALGORITHM_HPP
