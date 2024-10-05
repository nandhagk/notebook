#ifndef LIB_MONOID_SUMMAX_HPP
#define LIB_MONOID_SUMMAX_HPP 1

#include <utility>
#include <algorithm>
#include <lib/prelude.hpp>

template <class T>
struct monoid_summax {
        using X = std::pair<T, T>;
        using ValueT = X;

        static constexpr X op(const X &x, const X &y) {
                return {x.first + y.first, std::max(x.second, y.second)};
        }

        static constexpr X unit() {
                return {T(0), -inf<T>};
        }

        static constexpr bool commutative = true;
};

#endif // LIB_MONOID_SUMMAX_HPP
