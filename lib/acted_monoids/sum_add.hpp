#ifndef LIB_ACTED_MONOID_SUM_ADD_HPP
#define LIB_ACTED_MONOID_SUM_ADD_HPP 1

#include <lib/monoids/add.hpp>

template <typename T> struct acted_monoid_sum_add {
    using MX = monoid_add<T>;
    using MA = monoid_add<T>;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    static constexpr X act(X x, A a, i32 sz) { return x + a * sz; }
};

#endif // LIB_ACTED_MONOID_SUM_ADD_HPP
