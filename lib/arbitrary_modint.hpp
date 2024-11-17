#ifndef LIB_ARBITRARY_MODINT_HPP
#define LIB_ARBITRARY_MODINT_HPP 1

#include <iostream>

#include <lib/prelude.hpp>
#include <lib/math.hpp>

template <typename U, i32 id, is_unsigned_integral_t<U>* = nullptr>
struct arbitrary_modint_base {
	using mint = arbitrary_modint_base;

	constexpr arbitrary_modint_base(): v(0) {}

	template <typename T, is_unsigned_integral_t<T>* = nullptr>
	arbitrary_modint_base(T x): v(U(x % mod())) {}

	template <typename T, is_signed_integral_t<T>* = nullptr>
	arbitrary_modint_base(T x) {
		using S = make_signed<U>;

		S u = S(x % S(mod()));
		if (u < 0) u += mod();

		v = u;
	}

	static void set_mod(U m) {
		bt = m;
	}

	static U mod() { 
		return bt.mod();
	}

	U val() const {
		return v;
	}

	mint operator-() const {
		mint r;
		r.v = (v == 0 ? 0 : mod() - v);
		return r;
	}

	mint inv() const {
		const auto &[f, s] = inv_gcd(v, mod());
		assert(f == 1);

		return s;
	}

	mint pow(u64 n) const {
		return binpow(*this, n);
	}

	mint& operator+=(const mint& rhs) & {
		v += rhs.val();
		if (v >= mod()) v -= mod();
		return *this;
	}

	mint& operator-=(const mint& rhs) & {
		v -= rhs.val();
		if (v >= mod()) v -= mod();
		return *this;
	}

	mint& operator*=(const mint& rhs) & {
		v = bt.mul(v, rhs.val());
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

	friend bool operator==(const mint& lhs, const mint& rhs) {
		return lhs.val() == rhs.val();
	}

	friend bool operator!=(const mint& lhs, const mint& rhs) {
		return lhs.val() != rhs.val();
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
	static barrett<U> bt;
};

template <i32 id>
using arbitrary_modint_32 = arbitrary_modint_base<u32, id>;

template <>
inline barrett<u32> arbitrary_modint_32<-1>::bt = 998'244'353;

template <i32 id>
using arbitrary_modint_64 = arbitrary_modint_base<u64, id>;

template <>
inline barrett<u64> arbitrary_modint_64<-1>::bt = (u64(1) << 61) - 1;


#endif // LIB_ARBITRARY_MODINT_HPP
