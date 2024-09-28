#ifndef LIB_MONOID_ADD_HPP
#define LIB_MONOID_ADD_HPP 1

#include <lib/prelude.hpp>

template <class T>
struct MonoidAdd {
        using X = T;
        using ValueT = T;

        static constexpr X Op(const X &x, const X &y) {
                return x + y;
        }

        static constexpr X Inv(const X &x) {
                return -x;
        }

        static constexpr X Pow(const X &x, i64 n) {
                return X(n) * x;
        }

        static constexpr X Unit() {
                return X(0);
        }

        static constexpr bool commutative = true;
};

#endif // LIB_MONOID_ADD_HPP
