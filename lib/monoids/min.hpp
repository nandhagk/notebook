#ifndef LIB_MONOID_MIN_HPP
#define LIB_MONOID_MIN_HPP 1

#include <algorithm>
#include <lib/prelude.hpp>

template <typename T>
struct monoid_min {
    using X = T;
    using ValueT = T;

    static constexpr X op(X x, X y) { return std::min(x, y); }

    static constexpr X unit() { return inf<X>; }

    static constexpr bool commutative = true;
};

#endif // LIB_MONOID_MIN_HPP
