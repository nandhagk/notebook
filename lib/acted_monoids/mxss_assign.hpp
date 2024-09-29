#ifndef LIB_ACTED_MONOID_MXSS_ASSIGN_HPP
#define LIB_ACTED_MONOID_MXSS_ASSIGN_HPP 1

#include <lib/monoids/mxss.hpp>
#include <lib/monoids/assign.hpp>

template <typename T, T E>
struct ActedMonoidMXSSAssign {
	using MX = MonoidMXSS<T>;
	using MA = MonoidAssign<T, E>;

	using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	static constexpr X Act(const X &x, const A &a, const i64 size) {
		if (a == MA::Unit()) return x;

		if (a > 0) {
			return MX::FromElement(a * size);
		} else {
			return {a * size, T(0), T(0), T(0)};
		}
	}
};

#endif // LIB_ACTED_MONOID_MXSS_ASSIGN_HPP
