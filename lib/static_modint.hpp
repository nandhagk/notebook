#ifndef LIB_STATIC_MODINT_HPP
#define LIB_STATIC_MODINT_HPP 1

#include <iostream>

#include <lib/math.hpp>
#include <lib/miller_rabin.hpp>
#include <lib/prelude.hpp>

template <typename U, U m, is_unsigned_integral_t<U> * = nullptr>
struct static_modint_base {
    using mint = static_modint_base;

    constexpr static_modint_base() : v(0) {}

    template <typename T, is_unsigned_integral_t<T> * = nullptr>
    constexpr static_modint_base(T x) : v(U(x % mod())) {}

    template <typename T, is_signed_integral_t<T> * = nullptr>
    constexpr static_modint_base(T x) {
        using S = make_signed<U>;

        S u = S(x % S(mod()));
        if (u < 0) u += mod();

        v = u;
    }

    constexpr static U mod() { return m; }

    constexpr U val() const { return v; }

    constexpr mint inv() const {
        if constexpr (is_prime) {
            return pow(mod() - 2);
        } else {
            using S = make_signed<U>;
            return ::inv(S(val()), S(mod()));
        }
    }

    constexpr mint pow(u64 n) const { return binpow(*this, n); }

    constexpr mint &operator+=(const mint &rhs) & {
        if ((v += rhs.val()) >= mod()) v -= mod();
        return *this;
    }

    constexpr mint &operator-=(const mint &rhs) & {
        if ((v += mod() - rhs.val()) >= mod()) v -= mod();
        return *this;
    }

    constexpr mint &operator*=(const mint &rhs) & {
        v = mul<mod()>(v, rhs.val());
        return *this;
    }

    constexpr mint &operator/=(const mint &rhs) & { return *this *= rhs.inv(); }

    friend constexpr mint operator+(mint lhs, const mint &rhs) { return lhs += rhs; }

    friend constexpr mint operator-(mint lhs, const mint &rhs) { return lhs -= rhs; }

    friend constexpr mint operator*(mint lhs, const mint &rhs) { return lhs *= rhs; }

    friend constexpr mint operator/(mint lhs, const mint &rhs) { return lhs /= rhs; }

    constexpr mint operator-() const { return mint(0) - mint(*this); }

    friend constexpr bool operator==(const mint &lhs, const mint &rhs) { return lhs.val() == rhs.val(); }

    friend constexpr bool operator!=(const mint &lhs, const mint &rhs) { return lhs.val() != rhs.val(); }

    friend std::ostream &operator<<(std::ostream &os, const mint &rhs) { return os << rhs.val(); }

    friend std::istream &operator>>(std::istream &is, mint &rhs) {
        i64 x;
        is >> x;

        rhs = mint(x);
        return is;
    }

private:
    U v;
    inline static constexpr bool is_prime = is_prime_v<U, m>;
};

template <u32 m>
using static_modint_32 = static_modint_base<u32, m>;

template <u64 m>
using static_modint_64 = static_modint_base<u64, m>;

using modint998244353 = static_modint_32<998'244'353>;
using modint1000000007 = static_modint_32<1'000'000'007>;

#endif // LIB_STATIC_MODINT_HPP
