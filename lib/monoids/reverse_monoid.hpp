#ifndef LIB_MONOID_REVERSE_MONOID_HPP
#define LIB_MONOID_REVERSE_MONOID_HPP 1

template <class Monoid>
struct monoid_reverse_monoid {
        using X = Monoid::ValueT;
        using ValueT = X;

        static constexpr X op(const X &x, const X &y) {
                return Monoid::op(y, x);
        }

        static constexpr X unit() {
                return Monoid::unit();
        }

        static constexpr bool commutative = Monoid::commutative;
};

#endif // LIB_MONOID_REVERSE_MONOID_HPP