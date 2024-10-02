#ifndef LIB_MONOID_AND_HPP
#define LIB_MONOID_AND_HPP 1

template <class T>
struct monoid_and {
        using X = T;
        using ValueT = T;

        static constexpr X op(const X &x, const X &y) {
                return x & y;
        }

        static constexpr X unit() {
                return X(-1);
        }

        static constexpr bool commutative = true;
};

#endif // LIB_MONOID_AND_HPP
