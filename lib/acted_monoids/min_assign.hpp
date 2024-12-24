#ifndef LIB_ACTED_MONOID_MIN_ASSIGN_HPP
#define LIB_ACTED_MONOID_MIN_ASSIGN_HPP 1

#include <lib/monoids/assign.hpp>
#include <lib/monoids/min.hpp>

template <typename T> struct acted_monoid_min_assign {
    using MX = monoid_min<T>;
    using MA = monoid_assign<T>;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    static constexpr X act(X x, A a, i32) { return a ? *a : x; }
};

#endif // LIB_ACTED_MONOID_MIN_ASSIGN_HPP
