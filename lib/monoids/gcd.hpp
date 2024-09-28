#ifndef LIB_MONOID_GCD_HPP
#define LIB_MONOID_GCD_HPP 1

#include <numeric>

template <class T>
struct MonoidGCD {
        using X = T;
        using ValueT = T;

        static constexpr X Op(const X &x, const X &y) {
                return std::gcd(x, y);
        }

        static constexpr X Unit() {
                return X(0);
        }

        static constexpr bool commutative = true;
};

#endif // LIB_MONOID_GCD_HPP
