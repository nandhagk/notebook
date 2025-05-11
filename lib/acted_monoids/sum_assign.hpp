#ifndef LIB_ACTED_MONOID_SUM_ASSIGN_HPP
#define LIB_ACTED_MONOID_SUM_ASSIGN_HPP 1

#include <lib/monoids/add.hpp>
#include <lib/monoids/assign.hpp>

template <typename T>
struct acted_monoid_sum_assign {
    using MX = monoid_add<T>;
    using MA = monoid_assign<typename MX::ValueT>;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    static constexpr X act(const X &x, const A &a, i32 sz) {
        return a ? MX::pow(*a, sz) : x;
    }
};

#endif // LIB_ACTED_MONOID_SUM_ASSIGN_HPP
