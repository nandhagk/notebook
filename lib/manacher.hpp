#ifndef LIB_MANACHER_HPP
#define LIB_MANACHER_HPP

#include <vector>
#include <string>
#include <algorithm>

#include <lib/prelude.hpp>

template <class T>
std::pair<std::vector<i32>, std::vector<i32>> manacher(const std::vector<T> &s) {
        const i32 n = static_cast<i32>(s.size());

        // odd[i] = length of palindrome centered at i
        // even[i] = length of palindrome centered at space before i
        std::vector<i32> odd(n), even(n);
        for (i32 i = 0, l = 0, r = -1; i < n; ++i) {
                i32 len = (i > r) ? 1 : std::min(odd[l + r - i], r - i + 1);
                while (len <= i && i + len < n && s[i - len] == s[i + len]) ++len;
                odd[i] = len;
                --len;
                if (i + len > r) {
                        l = i - len;
                        r = i + len;
                }
        }

        for (i32 i = 0, l = 0, r = -1; i < n; ++i) {
                i32 len = (i > r) ? 0 : std::min(even[l + r - i + 1], r - i + 1);
                while (len < i && i + len < n && s[i - len - 1] == s[i + len]) ++len;
                even[i] = len;
                --len;
                if (i + len > r) {
                        l = i - len - 1;
                        r = i + len;
                }
        }

        return {odd, even};
}

inline std::pair<std::vector<i32>, std::vector<i32>> manacher(const std::string &s) {
        const i32 n = static_cast<i32>(s.size());

        std::vector<i32> t(n);
        for (i32 i = 0; i < n; ++i) t[i] = s[i];

        return manacher(t);
}

#endif // LIB_MANACHER_HPP
