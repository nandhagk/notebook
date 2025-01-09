#ifndef LIB_MATH_HPP
#define LIB_MATH_HPP 1

#include <cassert>
#include <type_traits>
#include <utility>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename T>
constexpr T binpow(T a, u64 b) {
    T r = 1;
    for (; b != 0; b >>= 1, a *= a)
        if (b & 1) r *= a;

    return r;
}

template <u32 m>
constexpr u32 mul(u32 a, u32 b) {
    return static_cast<u32>((u64(a) * b) % m);
}

template <u64 m>
constexpr u64 mul(u64 a, u64 b) {
    return static_cast<u64>((u128(a) * b) % m);
}

template <typename T>
constexpr T safe_mod(T x, T m) {
    x %= m;
    if (x < 0) x += m;
    return x;
}

template <typename T, is_signed_integral_t<T> * = nullptr>
constexpr std::pair<T, T> inv_gcd(T a, T b) {
    a = safe_mod(a, b);
    if (a == 0) return {b, 0};

    T s = b, t = a;
    T m0 = 0, m1 = 1;

    while (t) {
        T u = s / t;
        s -= t * u;
        m0 -= m1 * u;

        T tmp = s;
        s = t;
        t = tmp;

        tmp = m0;
        m0 = m1;
        m1 = tmp;
    }

    if (m0 < 0) m0 += b / s;
    return {s, m0};
}

template <typename T, is_signed_integral_t<T> * = nullptr>
constexpr T inv(T a, T b) {
    const auto &[f, s] = inv_gcd(a, b);
    assert(f == 1);

    return s;
}

template <typename T>
constexpr T modpow(T a, i64 n, T p) {
    using U = make_double<T>;

    a = safe_mod(a, p);
    T r = 1 % p;

    for (; n != 0; n >>= 1) {
        if (n & 1) r = T(U(r) * a % p);
        a = T(U(a) * a % p);
    }

    return r;
}

#endif // LIB_MATH_HPP
