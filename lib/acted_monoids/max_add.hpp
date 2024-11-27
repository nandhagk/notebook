#ifndef LIB_ACTED_MONOID_MAX_ADD_HPP
#define LIB_ACTED_MONOID_MAX_ADD_HPP 1

#include <lib/monoids/add.hpp>
#include <lib/monoids/max.hpp>

template <typename T>
struct acted_monoid_max_add {
	using MX = monoid_max<T>;
	using MA = monoid_add<T>;

	using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	static constexpr X act(const X &x, const A &a, i32) {
		return x == MX::unit() ? x : x + a;
	}
};

#endif // LIB_ACTED_MONOID_MAX_ADD_HPP
