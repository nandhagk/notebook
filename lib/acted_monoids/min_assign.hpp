#ifndef LIB_ACTED_MONOID_MIN_ASSIGN_HPP
#define LIB_ACTED_MONOID_MIN_ASSIGN_HPP 1

#include <lib/monoids/assign.hpp>
#include <lib/monoids/min.hpp>

template <typename T, T E>
struct acted_monoid_min_assign {
	using MX = monoid_min<T>;
	using MA = monoid_assign<T, E>;

	using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	static constexpr X act(const X &x, const A &a) {
		return x == MX::unit() ? x : a;
	}
};

#endif // LIB_ACTED_MONOID_MIN_ASSIGN_HPP
