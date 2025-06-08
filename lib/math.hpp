#ifndef LIB_MATH_HPP
#define LIB_MATH_HPP 1

#include <cassert>
#include <utility>

#include <lib/numeric_traits.hpp>
#include <lib/prelude.hpp>

template <typename T>
constexpr T binpow(T a, u64 n) {
    T r = 1;
    for (; n != 0; n >>= 1, a *= a)
        if (n & 1) r *= a;

    return r;
}

template <typename T, is_integral_t<T> * = nullptr>
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

template <typename T, is_integral_t<T> * = nullptr>
constexpr T modmul(T a, T b, T m) {
    return static_cast<T>((make_double<T>(a) * b) % m);
}

template <typename T, T m, is_integral_t<T> * = nullptr>
constexpr T modmul(T a, T b) {
    return static_cast<T>((make_double<T>(a) * b) % m);
}

template <typename T, is_integral_t<T> * = nullptr>
constexpr T modpow(T a, u64 n, T p) {
    T r = 1 % p;
    for (a = safe_mod(a, p); n != 0; n >>= 1, a = modmul(a, a, p))
        if (n & 1) r = modmul(r, a, p);

    return r;
}

template <typename EuclideanRing>
constexpr EuclideanRing gcd(EuclideanRing a, EuclideanRing b) {
    while (b != EuclideanRing()) {
        a %= b;
        std::swap(a, b);
    }

    return a;
}

#endif // LIB_MATH_HPP
