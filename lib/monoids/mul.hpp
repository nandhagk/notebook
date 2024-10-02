#ifndef LIB_MONOID_MUL_HPP
#define LIB_MONOID_MUL_HPP 1

template <class T>
struct monoid_mul {
        using X = T;
        using ValueT = T;

        static constexpr X op(const X &x, const X &y) {
                return x * y;
        }

        static constexpr X inv(const X &x) {
                return X(1) / x;
        }

        static constexpr X unit() {
                return X(1);
        }

        static constexpr bool commutative = true;
};

#endif // LIB_MONOID_MUL_HPP
