#ifndef LIB_ARBITRARY_MONTGOMERY_MODINT_HPP
#define LIB_ARBITRARY_MONTGOMERY_MODINT_HPP 1

#include <iostream>
#include <limits>

#include <lib/prelude.hpp>
#include <lib/math.hpp>

template <typename U, i32 id, is_unsigned_integral_t<U>* = nullptr>
struct arbitrary_montgomery_modint_base {
	using mint = arbitrary_montgomery_modint_base;

	using V = make_double<U>;
	using S = make_signed<U>;
	using T = make_signed<V>;

	static constexpr i32 W = std::numeric_limits<U>::digits;

	constexpr arbitrary_montgomery_modint_base(): v(0) {}

	template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
	arbitrary_montgomery_modint_base(T x):
		v(reduce(V(x % m + m) * n2)) {}

	static U reduce(V b) {
		return static_cast<U>((b + V(U(b) * U(-r)) * m) >> W);
	}

	static U get_r() {
		U p = m;
		while (m * p != 1) p *= U(2) - m * p;
		return p;
	}

	static void set_mod(U m_) {
		assert(m_ & 1 && m_ <= U(1) << (W - 2));

		m = m_;
		n2 = static_cast<U>(-V(m) % m);
		r = get_r();
	}

	U val() const {
		U p = reduce(v);
		return p >= m ? p - m : p;
	}

	static U mod() {
		return m;
	}

	mint inv() const {
		const auto &[f, s] = inv_gcd(val(), mod());
		assert(f == 1);

		return s;
	}

	mint pow(u64 n) const {
		return binpow(*this, n);
	}

	mint& operator+=(const mint& rhs) & {
		if (S(v += rhs.v - 2 * m) < 0) v += 2 * m;
		return *this;
	}

	mint& operator-=(const mint& rhs) & {
		if (S(v -= rhs.v) < 0) v += 2 * m;
		return *this;
	}

	mint& operator*=(const mint& rhs) & {
		v = reduce(V(v) * rhs.v);
		return *this;
	}

	mint& operator/=(const mint& rhs) & {
		return *this *= rhs.inv();
	}

	friend mint operator+(mint lhs, const mint& rhs) {
		return lhs += rhs;
	}

	friend mint operator-(mint lhs, const mint& rhs) {
		return lhs -= rhs;
	}

	friend mint operator*(mint lhs, const mint& rhs) {
		return lhs *= rhs;
	}

	friend mint operator/(mint lhs, const mint& rhs) {
		return lhs /= rhs;
	}

	mint operator-() const {
		return mint(0) - mint(*this);
	}

	friend bool operator==(const mint& lhs, const mint& rhs) {
		return (lhs.v >= m ? lhs.v - m : lhs.v) == (rhs.v >= m ? rhs.v - m : rhs.v);
	}

	friend bool operator!=(const mint& lhs, const mint& rhs) {
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
	inline static U m, r, n2;
};

template <i32 id>
using arbitrary_montgomery_modint_32 = arbitrary_montgomery_modint_base<u32, id>;

template <i32 id>
using arbitrary_montgomery_modint_64 = arbitrary_montgomery_modint_base<u64, id>;

#endif // LIB_ARBITRARY_MONTGOMERY_MODINT_HPP
