#ifndef LIB_MONOID_MUL_HPP
#define LIB_MONOID_MUL_HPP 1

#include <lib/prelude.hpp>

template <typename T>
struct monoid_mul {
    using X = T;
    using ValueT = T;

    static constexpr X op(X x, X y) {
        return x * y;
    }

    static constexpr X inv(X x) {
        return X(1) / x;
    }

    static constexpr X pow(X x, i64 n) {
        X z = 1;

        X y = x;
        while (n) {
            if (n & 1) z *= y;
            y *= y;
            n >>= 1;
        }

        return z;
    }

    static constexpr X unit() {
        return X(1);
    }

    static constexpr bool commutative = true;
};

#endif // LIB_MONOID_MUL_HPP
