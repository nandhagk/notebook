#ifndef LIB_ACTED_MONOID_MNSS_ASSIGN_HPP
#define LIB_ACTED_MONOID_MNSS_ASSIGN_HPP 1

#include <lib/monoids/mnss_sz.hpp>
#include <lib/monoids/assign.hpp>

template <typename T, T E>
struct acted_monoid_mnss_assign {
	using MX = monoid_mnss_sz<T>;
	using MA = monoid_assign<T, E>;

	using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	static constexpr X act(const X &x, const A &a) {
		if (a == MA::unit()) return x;
		return MX::from_element(a * std::get<4>(x));
	}
};

#endif // LIB_ACTED_MONOID_MNSS_ASSIGN_HPP
