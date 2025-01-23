#ifndef LIB_MILLER_RABIN_HPP
#define LIB_MILLER_RABIN_HPP 1

#include <lib/math.hpp>
#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

constexpr bool miller_rabin(u32 n) {
    if (n <= 2) return n == 2;
    if (n % 2 == 0) return false;

    u32 d = n - 1;
    while (d % 2 == 0) d >>= 1;

    u64 e = 1, r = n - 1;
    for (const u32 a : {2, 7, 61}) {
        if (a % n == 0) continue;

        u64 t = d;
        u64 y = modpow<u32>(a, t, n);
        while (t != n - 1 && y != e && y != r) {
            y = u32(u64(y) * y % n);
            t <<= 1;
        }

        if (y != r && t % 2 == 0) return false;
    }

    return true;
}

constexpr bool miller_rabin(u64 n) {
    if (n <= 2) return n == 2;
    if (n % 2 == 0) return false;

    u64 d = n - 1;
    while (d % 2 == 0) d >>= 1;

    u128 e = 1, r = n - 1;
    for (const u64 a : {2, 325, 9375, 28178, 450775, 9780504, 1795265022}) {
        if (a % n == 0) continue;

        u64 t = d;
        u128 y = modpow<u64>(a, t, n);
        while (t != n - 1 && y != e && y != r) {
            y = u64(u128(y) * y % n);
            t <<= 1;
        }

        if (y != r && t % 2 == 0) return false;
    }

    return true;
}

template <typename U, U m, is_unsigned_integral_t<U> * = nullptr>
constexpr bool is_prime_v = miller_rabin(m);

template <typename T, is_integral_t<T> * = nullptr>
inline bool is_prime(T n) {
    if (n < (1 << 30))
        return miller_rabin(static_cast<u32>(n));
    else
        return miller_rabin(static_cast<u64>(n));
}

#endif // LIB_MILLER_RABIN_HPP
