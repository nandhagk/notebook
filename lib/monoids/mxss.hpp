#ifndef LIB_MONOID_MXSS_HPP
#define LIB_MONOID_MXSS_HPP 1

#include <algorithm>
#include <lib/prelude.hpp>
#include <tuple>

template <typename T> struct monoid_mxss {
    using X = std::tuple<T, T, T, T>;
    using ValueT = X;

    static constexpr X op(X x, X y) {
        if (x == unit()) return y;
        if (y == unit()) return x;

        const auto [a, b, c, d] = x;
        const auto [p, q, r, s] = y;

        return {a + p, std::max(b, a + q), std::max(r, c + p), std::max({d, s, c + q})};
    }

    static constexpr X from_element(const T &t) { return {t, t, t, t}; }

    static constexpr X unit() { return from_element(-inf<T>); }

    static constexpr bool commutative = false;
};

#endif // LIB_MONOID_MXSS_HPP
