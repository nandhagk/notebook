#ifndef LIB_MONOID_MNSS_HPP
#define LIB_MONOID_MNSS_HPP 1

#include <tuple>
#include <algorithm>
#include <lib/prelude.hpp>

template <class T>
struct MonoidMNSS {
        using X = std::tuple<T, T, T, T>;
        using ValueT = X;

        static constexpr X Op(const X &x, const X &y) {
                const auto [a, b, c, d] = x;
                const auto [p, q, r, s] = y;

                return {a + p, std::min(b, a + q), std::min(r, c + p), std::min({d, s, c + q})};
        }

        static constexpr X FromElement(const T& t) {
                return {t, t, t, t};
        }

        static constexpr X Unit() {
                return FromElement(T(0));
        }

        static constexpr bool commutative = false;
};

#endif // LIB_MONOID_MNSS_HPP
