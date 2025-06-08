#ifndef LIB_POLLARD_RHO_HPP
#define LIB_POLLARD_RHO_HPP 1

#include <numeric>

#include <lib/arbitrary_montgomery_modint.hpp>
#include <lib/math.hpp>
#include <lib/miller_rabin.hpp>
#include <lib/numeric_traits.hpp>
#include <lib/prelude.hpp>
#include <lib/random.hpp>

template <typename U, is_unsigned_integral_t<U> * = nullptr>
inline U pollard_rho(U n) {
    if (n % 2 == 0) return 2;
    if (is_prime(n)) return n;

    using Z = arbitrary_montgomery_modint_base<U, -1>;
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
                g = gcd(q.val(), n);
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

#endif // LIB_POLLARD_RHO_HPP 1
