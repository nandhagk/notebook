#ifndef LIB_ACTED_MONOID_AFFINE_ASSIGN_HPP
#define LIB_ACTED_MONOID_AFFINE_ASSIGN_HPP 1

#include <lib/monoids/affine.hpp>
#include <lib/monoids/assign.hpp>

template <typename T, typename monoid_affine<T>::X E>
struct acted_monoid_affine_assign {
	using MX = monoid_affine<T>;
	using MA = monoid_assign<typename MX::ValueT, E>;

	using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	static constexpr X act(const X &x, const A &a, i32 sz) {
		if (a == MA::unit()) return x;
		return MX::pow(a, sz);
	}
};

#endif // LIB_ACTED_MONOID_AFFINE_ASSIGN_HPP
