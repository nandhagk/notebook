#ifndef LIB_ACTED_MONOID_SUM_AFFINE_HPP
#define LIB_ACTED_MONOID_SUM_AFFINE_HPP 1

#include <lib/monoids/add.hpp>
#include <lib/monoids/affine.hpp>

template <typename T>
struct acted_monoid_sum_affine {
    using MX = monoid_add<T>;
    using MA = monoid_affine<T>;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    static constexpr X act(const X &x, const A &a, i32 sz) {
        const auto &[p, q] = a;
        return x * p + q * sz;
    }
};

#endif // LIB_ACTED_MONOID_SUM_AFFINE_HPP
