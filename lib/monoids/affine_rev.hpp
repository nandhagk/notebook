#ifndef LIB_MONOID_AFFINE_REV_HPP
#define LIB_MONOID_AFFINE_REV_HPP 1

#include <tuple>

#include <lib/prelude.hpp>

template <typename T>
struct monoid_affine_rev {
    using X = std::tuple<T, T, T>;
    using ValueT = X;

    static constexpr X op(const X &x, const X &y) {
        const auto &[a, b, c] = x;
        const auto &[p, q, r] = y;

        return {a * p, p * b + q, a * r + c};
    }

    static constexpr T eval(const X &f, T x) {
        const auto &[a, b, _] = f;
        return a * x + b;
    }

    static constexpr X unit() {
        return {T(1), T(0), T(0)};
    }

    static constexpr X pow(const X &x, i64 n) {
        X z = unit();
        for (X y = x; n; n >>= 1, y = op(y, y))
            if (n & 1) z = op(z, y);

        return z;
    }

    static constexpr X rev(const X &x) {
        const auto &[a, b, c] = x;
        return {a, c, b};
    }

    static constexpr bool commutative = false;
};

#endif // LIB_MONOID_AFFINE_REV_HPP
