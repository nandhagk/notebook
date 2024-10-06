#ifndef LIB_ACTED_MONOID_MAX_ASSIGN_HPP
#define LIB_ACTED_MONOID_MAX_ASSIGN_HPP 1

#include <lib/monoids/assign.hpp>
#include <lib/monoids/max.hpp>

template <typename T, T E>
struct acted_monoid_max_assign {
	using MX = monoid_max<T>;
	using MA = monoid_assign<T, E>;

	using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	static constexpr X act(const X &x, const A &a, const i64) {
		return x == MX::unit() ? x : a;
	}
};

#endif // LIB_ACTED_MONOID_MAX_ASSIGN_HPP
