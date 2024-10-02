#ifndef LIB_MONOID_MIN_CNT_HPP
#define LIB_MONOID_MIN_CNT_HPP 1

#include <utility>
#include <lib/prelude.hpp>

template <class T>
struct monoid_min_cnt {
        using X = std::pair<T, i32>;
        using ValueT = X;

        static constexpr X op(const X &x, const X &y) {
                const auto &[a, b] = x;
                const auto &[p, q] = y;

                if (a < p) return x;
                if (p < a) return y;

                return {a, b + q};
        }

        static constexpr X unit() {
                return {inf<T>, 0};
        }

        static constexpr X from_element(const T& t) {
                return {t, 1};
        }

        static constexpr bool commutative = true;
};

#endif // LIB_MONOID_MIN_CNT_HPP
