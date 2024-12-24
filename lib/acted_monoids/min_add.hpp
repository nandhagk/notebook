#ifndef LIB_ACTED_MONOID_MIN_ADD_HPP
#define LIB_ACTED_MONOID_MIN_ADD_HPP 1

#include <lib/monoids/add.hpp>
#include <lib/monoids/min.hpp>

template <typename T> struct acted_monoid_min_add {
    using MX = monoid_min<T>;
    using MA = monoid_add<T>;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    static constexpr X act(X x, A a, i32) { return x == MX::unit() ? x : x + a; }
};

#endif // LIB_ACTED_MONOID_MIN_ADD_HPP
