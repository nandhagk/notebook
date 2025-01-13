#ifndef LIB_ACTED_MONOID_MNSS_ASSIGN_HPP
#define LIB_ACTED_MONOID_MNSS_ASSIGN_HPP 1

#include <lib/monoids/assign.hpp>
#include <lib/monoids/mnss.hpp>

template <typename T>
struct acted_monoid_mnss_assign {
    using MX = monoid_mnss<T>;
    using MA = monoid_assign<T>;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    static constexpr X act(const X &x, const A &a, i32 sz) {
        return a ? MX::from_element(*a * sz) : x;
    }
};

#endif // LIB_ACTED_MONOID_MNSS_ASSIGN_HPP
