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

	static constexpr X act(const X &x, const A &a, const i64 size) {
		if (a == MA::unit()) return x;

		if (a < 0) {
			return MX::from_element(a * size);
		} else {
			return {a * size, T(0), T(0), T(0)};
		}
	}
};

#endif // LIB_ACTED_MONOID_MNSS_ASSIGN_HPP
