#ifndef LIB_MONOID_MUL_HPP
#define LIB_MONOID_MUL_HPP 1

template <class T>
struct MonoidMul {
        using X = T;
        using ValueT = T;

        static constexpr X Op(const X &x, const X &y) {
                return x * y;
        }

        static constexpr X Inv(const X &x) {
                return X(1) / x;
        }

        static constexpr X Unit() {
                return X(1);
        }

        static constexpr bool commutative = true;
};

#endif // LIB_MONOID_MUL_HPP
