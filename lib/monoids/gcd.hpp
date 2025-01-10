#ifndef LIB_MONOID_GCD_HPP
#define LIB_MONOID_GCD_HPP 1

#include <numeric>

template <typename T>
struct monoid_gcd {
    using X = T;
    using ValueT = T;

    static constexpr X op(X x, X y) {
        return std::gcd(x, y);
    }

    static constexpr X unit() {
        return X(0);
    }

    static constexpr bool commutative = true;
};

#endif // LIB_MONOID_GCD_HPP
