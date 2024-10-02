#ifndef LIB_MONOID_XOR_HPP
#define LIB_MONOID_XOR_HPP 1

#include <lib/prelude.hpp>

template <class T>
struct monoid_xor {
        using X = T;
        using ValueT = T;

        static constexpr X op(const X &x, const X &y) {
                return x ^ y;
        }

        static constexpr X inv(const X &x) {
                return x;
        }

        static constexpr X pow(const X &x, i64 n) {
                return n & 1 ? x : X(0);
        }

        static constexpr X unit() {
                return X(0);
        }

        static constexpr bool commutative = true;
};

#endif // LIB_MONOID_XOR_HPP
