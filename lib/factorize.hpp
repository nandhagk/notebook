#ifndef LIB_FACTORIZE_HPP
#define LIB_FACTORIZE_HPP 1

#include <vector>
#include <algorithm>

#include <lib/prelude.hpp>
#include <lib/pollard_rho.hpp>

template <typename U, is_unsigned_integral_t<U>* = nullptr>
inline std::vector<std::pair<U, i32>> factorize(U m) {
        std::vector<std::pair<U, i32>> out;

        const auto dfs = [&](auto &&self, U n) -> void {
                if (n <= 1) return;

                U p;
                if (n <= (1 << 30)) {
                        p = pollard_rho(static_cast<u32>(n));
                } else {
                        p = pollard_rho(n);
                }

                i32 c{};
                do {
                        n /= p;
                        ++c;
                } while (n % p == 0);

                out.emplace_back(p, c);
                self(self, n);
        };

        dfs(dfs, m);

        std::sort(out.begin(), out.end());
        return out;
}

#endif // LIB_FACTORIZE_HPP

