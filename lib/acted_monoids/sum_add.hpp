#ifndef LIB_ACTED_MONOID_SUM_ADD_HPP
#define LIB_ACTED_MONOID_SUM_ADD_HPP 1

#include <lib/monoids/add.hpp>
#include <lib/monoids/add_sz.hpp>

template <typename T>
struct acted_monoid_sum_add {
	using MX = monoid_add_sz<T>;
	using MA = monoid_add<T>;

	using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	static constexpr X act(const X &x, const A &a) {
		const auto [p, q] = x;
		return {p + q * a, q};
	}
};

#endif // LIB_ACTED_MONOID_SUM_ADD_HPP
