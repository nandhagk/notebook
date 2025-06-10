#ifndef LIB_RATIONAL_HPP
#define LIB_RATIONAL_HPP 1

#include <iostream>

#include <lib/math.hpp>

template <typename T, bool AUTO_REDUCE = true>
struct rational {
    T num, den;

    constexpr rational()
        : rational(0, 1) {}

    constexpr rational(T v)
        : rational(v, 1) {}

    constexpr rational(T n, T d)
        : num{n}, den{d} {
        if (den < 0) {
            num = -num;
            den = -den;
        }

        if (AUTO_REDUCE) reduce();
    }

    constexpr rational &reduce() {
        const T g = gcd(num, std::abs(den));
        num /= g;
        den /= g;
        return *this;
    }

    constexpr rational &operator+=(const rational &rhs) & {
        if constexpr (!AUTO_REDUCE) {
            num = num * rhs.den + den * rhs.num;
            den *= rhs.den;
        } else {
            const T g = gcd(std::abs(den), std::abs(rhs.den));
            num = num * (rhs.den / g) + (den / g) * rhs.num;
            den *= (rhs.den / g);
            reduce();
        }

        return *this;
    }

    constexpr rational &operator-=(const rational &rhs) & {
        if constexpr (!AUTO_REDUCE) {
            num = num * rhs.den - den * rhs.num;
            den *= rhs.den;
        } else {
            const T g = gcd(std::abs(den), std::abs(rhs.den));
            num = num * (rhs.den / g) - (den / g) * rhs.num;
            den *= (rhs.den / g);
            reduce();
        }

        return *this;
    }

    constexpr rational &operator*=(const rational &rhs) & {
        if constexpr (!AUTO_REDUCE) {
            num *= rhs.num;
            den *= rhs.den;
        } else {
            const T g1 = gcd(num, std::abs(rhs.den));
            const T g2 = gcd(std::abs(den), rhs.num);
            num = (num / g1) * (rhs.num / g2);
            den = (den / g2) * (rhs.den / g1);
        }

        return *this;
    }

    constexpr rational &operator/=(const rational &rhs) & {
        if constexpr (!AUTO_REDUCE) {
            num *= rhs.den;
            den *= rhs.num;
        } else {
            const T g1 = gcd(num, rhs.num);
            const T g2 = gcd(std::abs(den), std::abs(rhs.den));
            num = (num / g1) * (rhs.den / g2);
            den = (den / g2) * (rhs.num / g1);
        }

        if (den < 0) {
            num = -num;
            den = -den;
        }

        return *this;
    }

    constexpr rational operator-() const {
        return {-num, den};
    }

    friend constexpr rational operator+(rational lhs, const rational &rhs) {
        return lhs += rhs;
    }

    friend constexpr rational operator-(rational lhs, const rational &rhs) {
        return lhs -= rhs;
    }

    friend constexpr rational operator*(rational lhs, const rational &rhs) {
        return lhs *= rhs;
    }

    friend constexpr rational operator/(rational lhs, const rational &rhs) {
        return lhs /= rhs;
    }

    friend constexpr bool operator==(const rational &lhs, const rational &rhs) {
        return lhs.num == rhs.num && lhs.den == rhs.den;
    }

    friend constexpr bool operator!=(const rational &lhs, const rational &rhs) {
        return !(lhs == rhs);
    }

    friend constexpr inline T cross(const rational &lhs, const rational &rhs) {
        const T g = gcd(std::abs(lhs.den), std::abs(rhs.den));
        return lhs.num * (rhs.den / g) - (lhs.den / g) * rhs.num;
    }

    friend constexpr bool operator<(const rational &lhs, const rational &rhs) {
        return cross(lhs, rhs) < 0;
    }

    friend constexpr bool operator<=(const rational &lhs, const rational &rhs) {
        return cross(lhs, rhs) <= 0;
    }

    friend constexpr bool operator>(const rational &lhs, const rational &rhs) {
        return cross(lhs, rhs) > 0;
    }

    friend constexpr bool operator>=(const rational &lhs, const rational &rhs) {
        return cross(lhs, rhs) >= 0;
    }

    friend std::istream &operator>>(std::istream &is, rational &r) {
        is >> r.num >> r.den;
        return is;
    }

    friend std::ostream &operator<<(std::ostream &os, const rational &r) {
        os << r.num << '/' << r.den;
        return os;
    }
};

#endif // LIB_RATIONAL_HPP
