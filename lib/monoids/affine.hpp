#ifndef LIB_MONOID_AFFINE_HPP
#define LIB_MONOID_AFFINE_HPP 1

#include <lib/prelude.hpp>
#include <utility>

template <typename T>
struct monoid_affine {
    using X = std::pair<T, T>;
    using ValueT = X;

    static constexpr X op(const X &x, const X &y) {
        return {x.first * y.first, x.second * y.first + y.second};
    }

    static constexpr X inv(const X &x) {
        auto [a, b] = x;
        a = T(1) / a;
        return {a, a * (-b)};
    }

    static constexpr T eval(const X &f, const T &x) {
        return f.first * x + f.second;
    }

    static constexpr X unit() {
        return {T(1), T(0)};
    }

    static constexpr X pow(const X &x, i64 n) {
        X z = unit();
        for (X y = x; n; n >>= 1, y = op(y, y))
            if (n & 1) z = op(z, y);

        return z;
    }

    static constexpr bool commutative = false;
};

#endif // LIB_MONOID_AFFINE_HPP
