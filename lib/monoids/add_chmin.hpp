#ifndef LIB_MONOID_ADD_CHMIN_HPP
#define LIB_MONOID_ADD_CHMIN_HPP 1

#include <algorithm>

#include <lib/limits.hpp>
#include <lib/prelude.hpp>

template <typename T>
struct monoid_add_chmin {
    using X = std::pair<T, T>;
    using ValueT = X;

    static constexpr X op(const X &x, const X &y) {
        auto [a, b] = x;
        auto [d, e] = y;

        a = a + d;
        b = (b == inf<T> ? b : b + d);
        b = std::min(b, e);

        return {a, b};
    }

    static constexpr T eval(const X &f, T x) {
        const auto &[a, b] = f;
        return std::min(x + a, b);
    }

    static constexpr X add(T a) {
        return {a, inf<T>};
    }

    static constexpr X chmin(T b) {
        return {0, b};
    }

    static constexpr X unit() {
        return {0, inf<T>};
    }

    static constexpr bool commutative = false;
};

#endif // LIB_MONOID_ADD_CHMIN_HPP
