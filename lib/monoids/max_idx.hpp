#ifndef LIB_MONOID_MAX_IDX_HPP
#define LIB_MONOID_MAX_IDX_HPP 1

#include <utility>

#include <lib/prelude.hpp>

template <typename T>
struct monoid_max_idx {
    using X = std::pair<T, i32>;
    using ValueT = X;

    static constexpr X op(const X &x, const X &y) {
        const auto &[a, b] = x;
        const auto &[p, q] = y;

        if (p > a) return y;
        return x;
    }

    static constexpr X unit() {
        return {-inf<T>, inf<T>};
    }

    static constexpr X from_element(T t, i32 p) {
        return {t, p};
    }

    static constexpr bool commutative = true;
};

#endif // LIB_MONOID_MAX_IDX_HPP
