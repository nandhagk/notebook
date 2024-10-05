#ifndef LIB_MONOID_MNSS_HPP
#define LIB_MONOID_MNSS_HPP 1

#include <tuple>
#include <algorithm>
#include <lib/prelude.hpp>

template <class T>
struct monoid_mnss {
        using X = std::tuple<T, T, T, T>;
        using ValueT = X;

        static constexpr X op(const X &x, const X &y) {
                if (x == unit()) return y;
                if (y == unit()) return x;

                const auto [a, b, c, d] = x;
                const auto [p, q, r, s] = y;

                return {a + p, std::min(b, a + q), std::min(r, c + p), std::min({d, s, c + q})};
        }

        static constexpr X from_element(const T& t) {
                return {t, t, t, t};
        }

        static constexpr X unit() {
                return from_element(inf<T>);
        }

        static constexpr bool commutative = false;
};

#endif // LIB_MONOID_MNSS_HPP
