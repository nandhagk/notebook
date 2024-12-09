#ifndef LIB_ACTED_MONOID_MXSS_ASSIGN_HPP
#define LIB_ACTED_MONOID_MXSS_ASSIGN_HPP 1

#include <lib/monoids/mxss.hpp>
#include <lib/monoids/assign.hpp>

template <typename T, T E>
struct acted_monoid_mxss_assign {
	using MX = monoid_mxss<T>;
	using MA = monoid_assign<T, E>;

	using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	static constexpr X act(X x, A a, i32 sz) {
		if (a == MA::unit()) return x;
		return MX::from_element(a * sz);
	}
};

#endif // LIB_ACTED_MONOID_MXSS_ASSIGN_HPP
