#ifndef LIB_MONOID_XOR_HPP
#define LIB_MONOID_XOR_HPP 1

#include <lib/prelude.hpp>

template <typename T>
struct monoid_xor {
    using X = T;
    using ValueT = T;

    static constexpr X op(X x, X y) { return x ^ y; }

    static constexpr X inv(X x) { return x; }

    static constexpr X pow(X x, i64 n) { return n & 1 ? x : X(0); }

    static constexpr X unit() { return X(0); }

    static constexpr bool commutative = true;
};

#endif // LIB_MONOID_XOR_HPP
