#ifndef LIB_MONOID_AND_HPP
#define LIB_MONOID_AND_HPP 1

template <typename T> struct monoid_and {
    using X = T;
    using ValueT = T;

    static constexpr X op(X x, X y) { return x & y; }

    static constexpr X unit() { return X(-1); }

    static constexpr bool commutative = true;
};

#endif // LIB_MONOID_AND_HPP
