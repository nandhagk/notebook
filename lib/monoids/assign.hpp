#ifndef LIB_MONOID_ASSIGN_HPP
#define LIB_MONOID_ASSIGN_HPP 1

template <class T, T E>
struct MonoidAssign {
        using X = T;
        using ValueT = T;

        static constexpr X Op(const X &x, const X &y) {
                return y == E ? x : y;
        }

        static constexpr X Unit() {
                return E;
        }

        static constexpr bool commutative = false;
};

#endif // LIB_MONOID_ASSIGN_HPP
