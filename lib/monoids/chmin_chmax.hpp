#ifndef LIB_MONOID_CHMIN_CHMAX_HPP
#define LIB_MONOID_CHMIN_CHMAX_HPP 1

#include <algorithm>

#include <lib/prelude.hpp>

template <typename T>
struct monoid_chmin_chmax {
    using X = std::pair<T, T>;
    using ValueT = X;

    static constexpr X op(const X &x, const X &y) {
        auto [b, c] = x;
        auto [e, f] = y;

        b = std::min(b, e);
        c = std::max(std::min(c, e), f);

        return {b, c};
    }

    static constexpr T eval(const X &f, T x) {
        const auto &[b, c] = f;
        return std::max(std::min(x, b), c);
    }

    static X chmin(T b) {
        return {b, -inf<T>};
    }

    static X chmax(T c) {
        return {inf<T>, c};
    }

    static constexpr X unit() {
        return {inf<T>, -inf<T>};
    }

    static constexpr bool commutative = false;
};

#endif // LIB_MONOID_CHMIN_CHMAX_HPP
