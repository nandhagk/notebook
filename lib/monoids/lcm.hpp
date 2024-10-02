#ifndef LIB_MONOID_LCM_HPP
#define LIB_MONOID_LCM_HPP 1

#include <numeric>

template <class T>
struct monoid_lcm {
        using X = T;
        using ValueT = T;

        static constexpr X op(const X &x, const X &y) {
                return std::lcm(x, y);
        }

        static constexpr X unit() {
                return X(1);
        }

        static constexpr bool commutative = true;
};

#endif // LIB_MONOID_LCM_HPP
