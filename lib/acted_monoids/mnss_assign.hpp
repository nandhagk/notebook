#ifndef LIB_ACTED_MONOID_MNSS_ASSIGN_HPP
#define LIB_ACTED_MONOID_MNSS_ASSIGN_HPP 1

#include <lib/monoids/mnss.hpp>
#include <lib/monoids/assign.hpp>

template <typename T, T E>
struct acted_monoid_mnss_assign {
	using MX = monoid_mnss<T>;
	using MA = monoid_assign<T, E>;

	using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	static constexpr X act(X x, A a, i32 sz) {
		if (a == MA::unit()) return x;
		return MX::from_element(a * sz);
	}
};

#endif // LIB_ACTED_MONOID_MNSS_ASSIGN_HPP
