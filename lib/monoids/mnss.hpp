#ifndef LIB_MONOID_MNSS_HPP
#define LIB_MONOID_MNSS_HPP 1

#include <algorithm>
#include <tuple>

#include <lib/limits.hpp>
#include <lib/prelude.hpp>

template <typename T>
struct monoid_mnss {
    using X = std::tuple<T, T, T, T>;
    using ValueT = X;

    static constexpr X op(X x, X y) {
        if (x == unit()) return y;
        if (y == unit()) return x;

        const auto [a, b, c, d] = x;
        const auto [p, q, r, s] = y;

        return {a + p, std::min(b, a + q), std::min(r, c + p), std::min({d, s, c + q})};
    }

    static constexpr X from_element(const T &t) {
        return {t, t, t, t};
    }

    static constexpr X unit() {
        return from_element(inf<T>);
    }

    static constexpr X rev(const X &x) {
        const auto &[a, b, c, d] = x;
        return {a, c, b, d};
    }

    static constexpr bool commutative = false;
};

#endif // LIB_MONOID_MNSS_HPP
