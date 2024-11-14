#ifndef LIB_FACTORIZE_HPP
#define LIB_FACTORIZE_HPP 1

#include <vector>
#include <algorithm>

#include <lib/prelude.hpp>
#include <lib/pollard_rho.hpp>

template <typename U, is_unsigned_integral_t<U>* = nullptr>
inline std::vector<U> factorize(U m) {
        std::vector<U> out;

        const auto inner = [&](auto &&self, U n) -> void {
                if (n <= 1) return;

                U p;
                if (n <= (1 << 30)) {
                        p = pollard_rho(static_cast<u32>(n));
                } else {
                        p = pollard_rho(n);
                }

                if (p == n) {
                        out.push_back(p);
                        return;
                }

                self(self, p);
                self(self, n / p);
        };

        inner(inner, m);

        std::sort(out.begin(), out.end());
        return out;
}

#endif // LIB_FACTORIZE_HPP

