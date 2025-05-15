#ifndef LIB_DE_BRUIJN_HPP
#define LIB_DE_BRUIJN_HPP 1

#include <vector>

#include <lib/prelude.hpp>

inline std::vector<i32> de_bruijn(i32 n, i32 k = 2) {
    std::vector<i32> s;

    std::vector<i32> a(k * n);
    const auto dfs = [&](auto &&self, i32 t, i32 p) -> void {
        if (t > n) {
            if (n % p == 0)
                for (i32 j = 1; j <= p; ++j) s.push_back(a[j]);

            return;
        }

        a[t] = a[t - p];
        self(self, t + 1, p);

        for (i32 j = a[t - p] + 1; j < k; ++j) {
            a[t] = j;
            self(self, t + 1, t);
        }
    };

    dfs(dfs, 1, 1);
    return s;
}

#endif // LIB_DE_BRUIJN_HPP
