#ifndef LIB_NT_HPP
#define LIB_NT_HPP 1

#include <vector>
#include <algorithm>

#include <lib/prelude.hpp>
#include <lib/random.hpp>
#include <lib/modint.hpp>

inline bool is_prime(u64 n) {
        if (n < (1 << 30)) {
                return miller_rabin(static_cast<u32>(n));
        } else {
                return miller_rabin(n);
        }
}

template <typename U, is_unsigned_integral_t<U>* = nullptr>
inline U pollard_rho(U n) {
        if (n % 2 == 0) return 2;
        if (is_prime(n)) return n;

        using Z = dynamic_montgomery_modint_base<U, -1>;
        Z::set_mod(n);

        Z R, one = 1;
        const auto f = [&](Z x) { return x * x + R; };
        const auto rnd = [&]() { return (MT() % (n - 2)) + 2; };

        for (;;) {
                Z x, y, ys, q = one;

                R = rnd();
                y = rnd();

                U g = 1;
                constexpr static const i32 m = 128;

                for (i32 r = 1; g == 1; r <<= 1) {
                        x = y;
                        for (i32 i = 0; i < r; ++i) y = f(y);
                        for (i32 k = 0; g == 1 && k < r; k += m) {
                                ys = y;
                                for (i32 i = 0; i < m && i < r - k; ++i) q *= x - (y = f(y));
                                g = std::gcd(q.val(), n); 
                        }
                }

                if (g == n) {
                        do { 
                                g = std::gcd((x - (ys = f(ys))).val(), n);
                        } while (g == 1);
                }

                if (g != n) return g;
        }

        assert(0);
}

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

#endif // LIB_NT_HPP

