#ifndef LIB_MONOID_MAX_HPP
#define LIB_MONOID_MAX_HPP 1

#include <algorithm>

#include <lib/limits.hpp>
#include <lib/prelude.hpp>

template <typename T>
struct monoid_max {
    using X = T;
    using ValueT = T;

    static constexpr X op(X x, X y) {
        return std::max(x, y);
    }

    static constexpr X unit() {
        return -inf<X>;
    }

    static constexpr X pow(X x, i64) {
        return x;
    }

    static constexpr bool commutative = true;
};

#endif // LIB_MONOID_MAX_HPP
