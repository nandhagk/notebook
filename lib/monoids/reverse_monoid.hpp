#ifndef LIB_MONOID_REVERSE_MONOID_HPP
#define LIB_MONOID_REVERSE_MONOID_HPP 1

#include <lib/algebraic_traits.hpp>

template <typename Monoid, is_monoid_t<Monoid> * = nullptr>
struct monoid_reverse_monoid {
    using MX = Monoid;

    using X = typename MX::ValueT;
    using ValueT = X;

    static constexpr X op(const X &x, const X &y) {
        return MX::op(y, x);
    }

    static constexpr X unit() {
        return MX::unit();
    }

    static constexpr X inv(const X &x) {
        return MX::inv(x);
    }

    static constexpr X pow(const X &x, i64 n) {
        return MX::pow(x, n);
    }

    static constexpr bool commutative = MX::commutative;
};

#endif // LIB_MONOID_REVERSE_MONOID_HPP
