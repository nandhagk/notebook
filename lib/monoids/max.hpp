#ifndef LIB_MONOID_MAX_HPP
#define LIB_MONOID_MAX_HPP 1

#include <algorithm>
#include <lib/prelude.hpp>

template <class T>
struct MonoidMax {
        using X = T;
        using ValueT = T;

        static constexpr X Op(const X &x, const X &y) {
                return std::max(x, y);
        }

        static constexpr X Unit() {
                return -inf<X>;
        }

        static constexpr bool commutative = true;
};

#endif // LIB_MONOID_MAX_HPP
