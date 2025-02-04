#ifndef LIB_MONOID_ADD_CHMIN_CHMAX_HPP
#define LIB_MONOID_ADD_CHMIN_CHMAX_HPP 1

#include <algorithm>

#include <lib/prelude.hpp>

template <typename T>
struct monoid_add_chmin_chmax {
    using X = std::tuple<T, T, T>;
    using ValueT = X;

    static constexpr X op(const X &x, const X &y) {
        auto [a, b, c] = x;
        auto [d, e, f] = y;

        a = a + d;
        b = (b == inf<T> ? b : b + d);
        c = (c == -inf<T> ? c : c + d);

        b = std::min(b, e);
        c = std::max(std::min(c, e), f);

        return {a, b, c};
    }

    static constexpr T eval(const X &f, T x) {
        const auto &[a, b, c] = f;
        return std::max(std::min(x + a, b), c);
    }

    static constexpr X add(T a) {
        return {a, inf<T>, -inf<T>};
    }

    static constexpr X chmin(T b) {
        return {0, b, -inf<T>};
    }

    static constexpr X chmax(T c) {
        return {0, inf<T>, c};
    }

    static constexpr X unit() {
        return {0, inf<T>, -inf<T>};
    }

    static constexpr bool commutative = false;
};

#endif // LIB_MONOID_ADD_CHMIN_CHMAX_HPP
