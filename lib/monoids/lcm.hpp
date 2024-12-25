#ifndef LIB_MONOID_LCM_HPP
#define LIB_MONOID_LCM_HPP 1

#include <numeric>

template <typename T>
struct monoid_lcm {
    using X = T;
    using ValueT = T;

    static constexpr X op(X x, X y) { return std::lcm(x, y); }

    static constexpr X unit() { return X(1); }

    static constexpr bool commutative = true;
};

#endif // LIB_MONOID_LCM_HPP
