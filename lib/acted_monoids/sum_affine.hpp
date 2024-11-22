#ifndef LIB_ACTED_MONOID_SUM_AFFINE_HPP
#define LIB_ACTED_MONOID_SUM_AFFINE_HPP 1

#include <lib/monoids/add_sz.hpp>
#include <lib/monoids/affine.hpp>

template <typename T>
struct acted_monoid_sum_affine {
	using MX = monoid_add_sz<T>;
	using MA = monoid_affine<T>;

	using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	static constexpr X act(const X &x, const A &a) {
		const auto [p, q] = x;
		const auto [b, c] = a;
		return {p * b + c * q, q};
	}
};

#endif // LIB_ACTED_MONOID_SUM_AFFINE_HPP
