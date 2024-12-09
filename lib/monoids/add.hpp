#ifndef LIB_MONOID_ADD_HPP
#define LIB_MONOID_ADD_HPP 1

#include <lib/prelude.hpp>

template <class T>
struct monoid_add {
        using X = T;
        using ValueT = T;

        static constexpr X op(X x, X y) {
                return x + y;
        }

        static constexpr X inv(X x) {
                return -x;
        }

        static constexpr X pow(X x, i64 n) {
                return X(n) * x;
        }

        static constexpr X unit() {
                return X(0);
        }

        static constexpr bool commutative = true;
};

#endif // LIB_MONOID_ADD_HPP
