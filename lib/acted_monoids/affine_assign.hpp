#ifndef LIB_ACTED_MONOID_AFFINE_ASSIGN_HPP
#define LIB_ACTED_MONOID_AFFINE_ASSIGN_HPP 1

#include <lib/monoids/affine_sz.hpp>
#include <lib/monoids/assign.hpp>

template <typename T, typename monoid_affine_sz<T>::X E>
struct acted_monoid_affine_assign {
	using MX = monoid_affine_sz<T>;
	using MA = monoid_assign<typename MX::ValueT, E>;

	using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	static constexpr X act(const X &x, const A &a) {
		if (a == MA::unit()) return x;
		return MX::pow(a, std::get<2>(x));
	}
};

#endif // LIB_ACTED_MONOID_AFFINE_ASSIGN_HPP
