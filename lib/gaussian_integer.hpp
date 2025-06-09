#ifndef LIB_GAUSSIAN_INTEGER_HPP
#define LIB_GAUSSIAN_INTEGER_HPP 1

#include <iostream>
#include <utility>

#include <lib/math.hpp>
#include <lib/numeric_traits.hpp>

template <typename T, is_signed_integral_t<T> * = nullptr>
struct gaussian_integer {
    using gint = gaussian_integer;

    T x, y;

    constexpr gaussian_integer(T a = 0, T b = 0)
        : x{a}, y{b} {}

    constexpr gaussian_integer(const std::pair<T, T> &p)
        : gaussian_integer(p.first, p.second) {}

    constexpr T norm() const {
        return x * x + y * y;
    }

    constexpr gint conjugate() const {
        return {x, -y};
    }

    constexpr gint &operator+=(const gint &rhs) & {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    constexpr gint &operator-=(const gint &rhs) & {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    constexpr gint &operator*=(const gint &rhs) & {
        const auto [a, b] = *this;
        x = a * rhs.x - b * rhs.y;
        y = a * rhs.y + b * rhs.x;
        return *this;
    }

    constexpr gint &operator/=(const gint &rhs) & {
        const T n = rhs.norm();
        *this *= rhs.conjugate();
        x = floor(x + n / 2, n);
        y = floor(y + n / 2, n);
        return *this;
    }

    constexpr gint &operator%=(const gint &rhs) & {
        *this -= (*this / rhs) * rhs;
        return *this;
    }

    constexpr gint operator-() const {
        return {-x, -y};
    }

    friend constexpr gint operator+(gint lhs, const gint &rhs) {
        return lhs += rhs;
    }

    friend constexpr gint operator-(gint lhs, const gint &rhs) {
        return lhs -= rhs;
    }

    friend constexpr gint operator*(gint lhs, const gint &rhs) {
        return lhs *= rhs;
    }

    friend constexpr gint operator/(gint lhs, const gint &rhs) {
        return lhs /= rhs;
    }

    friend constexpr gint operator%(gint lhs, const gint &rhs) {
        return lhs %= rhs;
    }

    friend constexpr bool operator==(const gint &lhs, const gint &rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    friend constexpr bool operator!=(const gint &lhs, const gint &rhs) {
        return !(lhs == rhs);
    }

    friend std::istream &operator>>(std::istream &is, gint &rhs) {
        is >> rhs.x >> rhs.y;
        return is;
    }

    friend std::ostream &operator<<(std::ostream &os, const gint &rhs) {
        os << rhs.x << ' ' << rhs.y;
        return os;
    }
};

#endif // LIB_GAUSSIAN_INTEGER_HPP
