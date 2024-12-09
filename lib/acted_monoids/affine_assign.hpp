#ifndef LIB_ACTED_MONOID_AFFINE_ASSIGN_HPP
#define LIB_ACTED_MONOID_AFFINE_ASSIGN_HPP 1

#include <lib/monoids/affine.hpp>
#include <lib/monoids/assign.hpp>

template <typename T>
struct acted_monoid_affine_assign {
	using MX = monoid_affine<T>;
	using MA = monoid_assign<T>;

	using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	static constexpr X act(X x, A a, i32 sz) {
		return a ? MX::pow(*a, sz) : x;
	}
};

#endif // LIB_ACTED_MONOID_AFFINE_ASSIGN_HPP
