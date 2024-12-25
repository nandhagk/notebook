#ifndef LIB_ACTED_MONOID_MXSS_ASSIGN_HPP
#define LIB_ACTED_MONOID_MXSS_ASSIGN_HPP 1

#include <lib/monoids/assign.hpp>
#include <lib/monoids/mxss.hpp>

template <typename T>
struct acted_monoid_mxss_assign {
    using MX = monoid_mxss<T>;
    using MA = monoid_assign<T>;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    static constexpr X act(X x, A a, i32 sz) { return a ? MX::from_element(*a * sz) : x; }
};

#endif // LIB_ACTED_MONOID_MXSS_ASSIGN_HPP
