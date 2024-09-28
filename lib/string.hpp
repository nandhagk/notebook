#ifndef LIB_STRING_HPP
#define LIB_STRING_HPP

#include <vector>
#include <string>
#include <lib/prelude.hpp>

template <class T>
std::vector<i32> z_algorithm(const std::vector<T>& s) {
        const i32 n = static_cast<i32>(s.size());
        if (n == 0) return {};

        std::vector<i32> z(n);
        z[0] = 0;

        for (i32 i = 1, j = 0; i < n; ++i) {
                i32& k = z[i];
                k = (j + z[j] <= i) ? 0 : std::min(j + z[j] - i, z[i - j]);

                while (i + k < n && s[k] == s[i + k]) ++k;
                if (j + z[j] < i + z[i]) j = i;
        }

        z[0] = n;
        return z;
}

inline std::vector<i32> z_algorithm(const std::string& s) {
        const i32 n = static_cast<i32>(s.size());

        std::vector<i32> s2(n);
        for (i32 i = 0; i < n; ++i) s2[i] = s[i];

        return z_algorithm(s2);
}

#endif // LIB_STRING_HPP
