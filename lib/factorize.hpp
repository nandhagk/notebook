#ifndef LIB_FACTORIZE_HPP
#define LIB_FACTORIZE_HPP 1

#include <algorithm>
#include <vector>

#include <lib/pollard_rho.hpp>
#include <lib/prelude.hpp>

template <typename U, is_unsigned_integral_t<U> * = nullptr>
inline std::vector<std::pair<U, i32>> factorize(U m) {
    std::vector<U> ps;

    const auto dfs = [&](auto &&self, U n) -> void {
        if (n <= 1) return;

        U p;
        if (n <= (1 << 30))
            p = pollard_rho(static_cast<u32>(n));
        else
            p = pollard_rho(n);

        if (p == n) {
            ps.push_back(p);
            return;
        }

        self(self, p);
        self(self, n / p);
    };

    dfs(dfs, m);

    std::sort(ps.begin(), ps.end());

    std::vector<std::pair<U, i32>> fs;
    for (const U p : ps)
        if (fs.empty() || fs.back().first != p)
            fs.emplace_back(p, 1);
        else
            ++fs.back().second;

    return fs;
}

#endif // LIB_FACTORIZE_HPP
