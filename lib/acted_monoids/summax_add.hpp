#ifndef LIB_ACTED_MONOID_SUMMAX_ADD_HPP
#define LIB_ACTED_MONOID_SUMMAX_ADD_HPP 1

#include <lib/monoids/add.hpp>
#include <lib/monoids/summax.hpp>

template <typename T>
struct acted_monoid_summax_add {
	using MX = monoid_summax<T>;
	using MA = monoid_add<T>;

	using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	static constexpr X act(const X &x, const A &a, const i64 size) {
		auto [p, q] = x;
		q = q == -inf<T> ? q : q + a;
		return {p + T(size) * a, q};
	}
};

#endif // LIB_ACTED_MONOID_SUM_ADD_HPP
