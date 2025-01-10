#ifndef LIB_MONOID_MAX_CNT_HPP
#define LIB_MONOID_MAX_CNT_HPP 1

#include <lib/prelude.hpp>
#include <utility>

template <typename T, typename S>
struct monoid_max_cnt {
    using X = std::pair<T, S>;
    using ValueT = X;

    static constexpr X op(const X &x, const X &y) {
        const auto &[a, b] = x;
        const auto &[p, q] = y;

        if (a > p) return x;
        if (p > a) return y;

        return {a, b + q};
    }

    static constexpr X unit() {
        return {-inf<T>, S(0)};
    }

    static constexpr bool commutative = true;
};

#endif // LIB_MONOID_MAX_CNT_HPP
