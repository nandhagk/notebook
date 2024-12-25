#ifndef LIB_MONOID_REVERSE_MONOID_HPP
#define LIB_MONOID_REVERSE_MONOID_HPP 1

template <typename Monoid>
struct monoid_reverse_monoid {
    using X = typename Monoid::ValueT;
    using ValueT = X;

    static constexpr X op(X x, X y) { return Monoid::op(y, x); }

    static constexpr X unit() { return Monoid::unit(); }

    static constexpr bool commutative = Monoid::commutative;
};

#endif // LIB_MONOID_REVERSE_MONOID_HPP
