#ifndef LIB_MONOID_MAX_IDX_HPP
#define LIB_MONOID_MAX_IDX_HPP 1

#include <utility>
#include <lib/prelude.hpp>

template <class T>
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

        static constexpr X from_element(const T& t, const i32 i) {
                return {t, i};
        }

        static constexpr bool commutative = true;
};

#endif // LIB_MONOID_MAX_CNT_HPP
