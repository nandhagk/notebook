#ifndef LIB_MONOID_OR_HPP
#define LIB_MONOID_OR_HPP 1

#include <lib/prelude.hpp>

template <typename T>
struct monoid_or {
    using X = T;
    using ValueT = T;

    static constexpr X op(X x, X y) {
        return x | y;
    }

    static constexpr X unit() {
        return X(0);
    }

    static constexpr X pow(X x, i64) {
        return x;
    }

    static constexpr bool commutative = true;
};

#endif // LIB_MONOID_OR_HPP
