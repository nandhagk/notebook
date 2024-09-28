#ifndef LIB_ACTED_MONOID_SUM_AFFINE_HPP
#define LIB_ACTED_MONOID_SUM_AFFINE_HPP 1

#include <lib/monoids/add.hpp>
#include <lib/monoids/affine.hpp>

template <typename T>
struct ActedMonoidSumAffine {
	using MX = MonoidAdd<T>;
	using MA = MonoidAffine<T>;

	using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	static constexpr X Act(const X &x, const A &a, const i64 size) {
		return x * a.first + T(size) * a.second;
	}
};

#endif // LIB_ACTED_MONOID_SUM_AFFINE_HPP
