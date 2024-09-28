#ifndef LIB_MONOID_OR_HPP
#define LIB_MONOID_OR_HPP 1

template <class T>
struct MonoidOr {
        using X = T;
        using ValueT = T;

        static constexpr X Op(const X &x, const X &y) {
                return x | y;
        }

        static constexpr X Unit() {
                return X(0);
        }

        static constexpr bool commutative = true;
};

#endif // LIB_MONOID_OR_HPP
