#ifndef LIB_ACTED_MONOID_MAX_CNT_ADD_HPP
#define LIB_ACTED_MONOID_MAX_CNT_ADD_HPP 1

#include <lib/monoids/add.hpp>
#include <lib/monoids/max_cnt.hpp>

template <typename T, typename S>
struct acted_monoid_max_cnt_add {
    using MX = monoid_max_cnt<T, S>;
    using MA = monoid_add<T>;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    static constexpr X act(const X &x, const A &a, i32) {
        return x == MX::unit() ? x : X{x.first + a, x.second};
    }
};

#endif // LIB_ACTED_MONOID_MAX_CNT_ADD_HPP
