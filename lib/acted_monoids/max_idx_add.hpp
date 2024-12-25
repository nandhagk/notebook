#ifndef LIB_ACTED_MONOID_MAX_IDX_ADD_HPP
#define LIB_ACTED_MONOID_MAX_IDX_ADD_HPP 1

#include <lib/monoids/add.hpp>
#include <lib/monoids/max_idx.hpp>

template <typename T>
struct acted_monoid_max_idx_add {
    using MX = monoid_max_idx<T>;
    using MA = monoid_add<T>;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    static constexpr X act(X x, A a, i32) { return x == MX::unit() ? x : X{x.first + a, x.second}; }
};

#endif // LIB_ACTED_MONOID_MAX_IDX_ADD_HPP
