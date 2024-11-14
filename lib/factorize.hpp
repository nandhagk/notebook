#ifndef LIB_FACTORIZE_HPP
#define LIB_FACTORIZE_HPP 1

#include <vector>
#include <algorithm>

#include <lib/prelude.hpp>
#include <lib/pollard_rho.hpp>

inline std::vector<i64> factorize(u64 m) {
        const auto inner = [](auto &&self, u64 n) -> std::vector<i64> {
                if (n <= 1) return {};

                u64 p;
                if (n <= (1 << 30)) {
                        p = pollard_rho(static_cast<u32>(n));
                } else {
                        p = pollard_rho(n);
                }

                if (p == n) return {i64(n)};

                auto l = self(self, p);
                auto r = self(self, n / p);
                        
                l.reserve(l.size() + r.size());
                std::copy(r.begin(), r.end(), std::back_inserter(l));

                return l;
        };

        auto out = inner(inner, m);
        std::sort(out.begin(), out.end());

        return out;
}

#endif // LIB_FACTORIZE_HPP

