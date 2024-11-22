#ifndef LIB_MONOID_ADD_SZ_HPP
#define LIB_MONOID_ADD_SZ_HPP 1

#include <utility>

#include <lib/prelude.hpp>

template <class T>
struct monoid_add_sz {
        using X = std::pair<T, i32>;
        using ValueT = X;

        static constexpr X op(const X &x, const X &y) {
                const auto [a, b] = x;
		const auto [p, q] = y;

		return {a + p, b + q};
        }

        static constexpr X inv(const X &x) {
		const auto [a, b] = x;
		return {-a, b};
        }

        static constexpr X pow(const X &x, i64 n) {
		const auto [a, b] = x;
		return {a * n, b};
        }

        static constexpr X unit() {
                return {0, 0};
        }

	static constexpr X from_element(const T& t) {
		return {t, 1};
	}

        static constexpr bool commutative = true;
};

#endif // LIB_MONOID_ADD_SZ_HPP
