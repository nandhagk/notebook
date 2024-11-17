#ifndef LIB_STATIC_MONTGOMERY_MODINT_HPP
#define LIB_STATIC_MONTGOMERY_MODINT_HPP 1

#include <limits>
#include <iostream>

#include <lib/prelude.hpp>
#include <lib/math.hpp>

template <typename U, U m, is_unsigned_integral_t<U>* = nullptr>
struct static_montgomery_modint_base {
	using mint = static_montgomery_modint_base;

	using V = make_double<U>;
	using S = make_signed<U>;
	using T = make_signed<V>;

	static constexpr i32 W = std::numeric_limits<U>::digits;
	static_assert(m & 1 && m <= U(1) << (W - 2));

	constexpr static U get_r() {
		U p = m;
		while (m * p != 1) p *= U(2) - m * p;
		return p;
	}

	static constexpr U n2 = static_cast<U>(-V(m) % m);
	static constexpr U r = get_r();

	constexpr static_montgomery_modint_base(): v(0) {}

	template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
	constexpr static_montgomery_modint_base(T x):
		v(reduce(V(x % m + m) * n2)) {}

	constexpr static U reduce(V b) {
		return static_cast<U>((b + V(U(b) * U(-r)) * m) >> W);
	}

	constexpr U val() const {
		U p = reduce(v);
		return p >= m ? p - m : p;
	}

	constexpr static U mod() {
		return m;
	}

	constexpr mint inv() const {
		return pow(mod() - 2);
	}

	constexpr mint pow(u64 n) const {
		return binpow(*this, n);
	}

	constexpr mint& operator+=(const mint& rhs) & {
		if (S(v += rhs.v - 2 * m) < 0) v += 2 * m;
		return *this;
	}

	constexpr mint& operator-=(const mint& rhs) & {
		if (S(v -= rhs.v) < 0) v += 2 * m;
		return *this;
	}

	constexpr mint& operator*=(const mint& rhs) & {
		v = reduce(V(v) * rhs.v);
		return *this;
	}

	constexpr mint& operator/=(const mint& rhs) & {
		return *this *= rhs.inv();
	}

	friend constexpr mint operator+(mint lhs, const mint& rhs) {
		return lhs += rhs;
	}

	friend constexpr mint operator-(mint lhs, const mint& rhs) {
		return lhs -= rhs;
	}

	friend constexpr mint operator*(mint lhs, const mint& rhs) {
		return lhs *= rhs;
	}

	friend constexpr mint operator/(mint lhs, const mint& rhs) {
		return lhs /= rhs;
	}

	constexpr mint operator-() const {
		return mint(0) - mint(*this);
	}

	friend constexpr bool operator==(const mint& lhs, const mint& rhs) {
		return (lhs.v >= m ? lhs.v - m : lhs.v) == (rhs.v >= m ? rhs.v - m : rhs.v);
	}

	friend constexpr bool operator!=(const mint& lhs, const mint& rhs) {
		return !(lhs == rhs);
	}

	friend std::ostream& operator<<(std::ostream& os, const mint& rhs) {
		return os << rhs.val();
	}

	friend std::istream& operator>>(std::istream& is, mint& rhs) {
		i64 x;
		is >> x;

		rhs = mint(x);
		return is;
	}

private:
	U v;
};

template <u32 m>
using static_montgomery_modint_32 = static_montgomery_modint_base<u32, m>;

template <u64 m>
using static_montgomery_modint_64 = static_montgomery_modint_base<u64, m>;

using montgomerymodint998244353 = static_montgomery_modint_32<998'244'353>;
using montgomerymodint1000000007 = static_montgomery_modint_32<1'000'000'007>;

#endif // LIB_STATIC_MONTGOMERY_MODINT_HPP
