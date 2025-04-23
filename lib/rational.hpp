#ifndef LIB_RATIONAL_HPP
#define LIB_RATIONAL_HPP 1

#include <iostream>
#include <numeric>

template <typename T, bool AUTO_REDUCE = true>
struct rational {
    T num, den;

    rational()
        : rational(0, 1) {}

    explicit rational(T v)
        : rational(v, 1) {}

    rational(T n, T d)
        : num{n}, den{d} {
        if (den < 0 || (den == 0 && num < 0)) {
            num = -num;
            den = -den;
        }

        reduce();
    }

    friend std::istream &operator>>(std::istream &is, rational &r) {
        is >> r.num >> r.den;
        return is;
    }

    friend std::ostream &operator<<(std::ostream &os, const rational &r) {
        os << r.num << '/' << r.den;
        return os;
    }

    friend rational operator+(const rational &lhs, const rational &rhs) {
        return {lhs.num * rhs.den + lhs.den * rhs.num, lhs.den * rhs.den};
    };

    friend rational operator-(const rational &lhs, const rational &rhs) {
        return {lhs.num * rhs.den - lhs.den * rhs.num, lhs.den * rhs.den};
    };

    friend rational operator*(const rational &lhs, const rational &rhs) {
        return {lhs.num * rhs.num, lhs.den * rhs.den};
    }

    friend rational operator/(const rational &lhs, const rational &rhs) {
        return {lhs.num * rhs.den, lhs.den * rhs.num};
    }

    friend inline T cross(const rational &lhs, const rational &rhs) {
        return lhs.num * rhs.den - lhs.den * rhs.num;
    }

    friend bool operator==(const rational &lhs, const rational &rhs) {
        return cross(lhs, rhs) == 0;
    }

    friend bool operator!=(const rational &lhs, const rational &rhs) {
        return cross(lhs, rhs) != 0;
    }

    friend bool operator<(const rational &lhs, const rational &rhs) {
        return cross(lhs, rhs) < 0;
    }

    friend bool operator<=(const rational &lhs, const rational &rhs) {
        return cross(lhs, rhs) <= 0;
    }

    friend bool operator>(const rational &lhs, const rational &rhs) {
        return cross(lhs, rhs) > 0;
    }

    friend bool operator>=(const rational &lhs, const rational &rhs) {
        return cross(lhs, rhs) >= 0;
    }

    rational &operator+=(const rational &rhs) {
        return *this = *this + rhs;
    }

    rational &operator-=(const rational &rhs) {
        return *this = *this - rhs;
    }

    rational &operator*=(const rational &rhs) {
        return *this = *this * rhs;
    }

    rational &operator/=(const rational &rhs) {
        return *this = *this / rhs;
    }

    rational operator+() const {
        return {+num, den};
    }

    rational operator-() const {
        return {-num, den};
    }

    rational &reduce() {
        const T g = std::gcd(num, den);
        num /= g;
        den /= g;

        return *this;
    }
};

#endif // LIB_RATIONAL_HPP
