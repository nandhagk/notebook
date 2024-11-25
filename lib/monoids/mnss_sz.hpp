#ifndef LIB_MONOID_MNSS_SZ_HPP
#define LIB_MONOID_MNSS_SZ_HPP 1

#include <tuple>
#include <algorithm>
#include <lib/prelude.hpp>

template <class T>
struct monoid_mnss_sz {
        using X = std::tuple<T, T, T, T, i32>;
        using ValueT = X;

        static constexpr X op(const X &x, const X &y) {
                const auto [a, b, c, d, e] = x;
                const auto [p, q, r, s, t] = y;

                if (x == unit()) return {p, q, r, s, t + e};
                if (y == unit()) return {a, b, c, d, t + e};

                return {a + p, std::min(b, a + q), std::min(r, c + p), std::min({d, s, c + q}), t + e};
        }

        static constexpr X from_element(const T& t) {
                return {t, t, t, t, 1};
        }

        static constexpr X unit() {
                return {inf<T>, inf<T>, inf<T>, inf<T>, 0};
        }

        static constexpr bool commutative = false;
};

#endif // LIB_MONOID_MNSS_SZ_HPP
