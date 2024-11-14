#ifndef LIB_MODINT_HPP
#define LIB_MODINT_HPP 1

#include <cassert>
#include <type_traits>
#include <iostream>
#include <utility>
#include <limits>

#include <lib/prelude.hpp>

template <typename T>
using is_signed_integral = typename std::conditional<
	std::is_integral<T>::value && std::is_signed<T>::value, std::true_type, std::false_type>::type;
	
template <typename T>
using is_unsigned_integral = typename std::conditional<
	std::is_integral<T>::value && std::is_unsigned<T>::value, std::true_type, std::false_type>::type;

template <typename T>
using is_signed_integral_t = std::enable_if_t<is_signed_integral<T>::value>;

template <typename T>
using is_unsigned_integral_t = std::enable_if_t<is_unsigned_integral<T>::value>;

template <typename T>
using make_signed = typename std::conditional<
	std::is_same_v<T, u128>, i128, typename std::conditional<std::is_same_v<T, u64>, i64, i32>::type>::type;

template <typename T>
using make_double = typename std::conditional<std::is_same_v<T, u64>, u128, u64>::type;

template <typename T>
constexpr T binpow(T a, u64 b, T r = 1) {
	for (; b != 0; b >>= 1, a *= a) {
		if (b & 1) r *= a;
	}

	return r;
}

template <u32 m>
constexpr u32 mul(u32 a, u32 b) {
	return static_cast<u32>((u64(a) * b) % m);
}

template <u64 m>
constexpr u64 mul(u64 a, u64 b) {
	return static_cast<u64>((u128(a) * b) % m);
}

constexpr i64 safe_mod(i64 x, i64 m) {
	x %= m;
	if (x < 0) x += m;
	return x;
}

constexpr std::pair<i64, i64> inv_gcd(i64 a, i64 b) {
	a = safe_mod(a, b);
	if (a == 0) return {b, 0};

	i64 s = b, t = a;
	i64 m0 = 0, m1 = 1;

	while (t) {
		i64 u = s / t;
		s -= t * u;
		m0 -= m1 * u;

		i64 tmp = s;
		s = t;
		t = tmp;

		tmp = m0;
		m0 = m1;
		m1 = tmp;
	}

	if (m0 < 0) m0 += b / s;
	return {s, m0};
}

struct barrett_32 {
	constexpr barrett_32(u32 m_): m(m_), im((u64)(-1) / m + 1) {}
	
	constexpr u32 mod() const {
		return m;
	}

	constexpr u32 modulo(u64 z) const {
		if (m == 1) return 0;

		u64 x = u64((u128(z) * im) >> 64);
		u64 y = x * m;

		return u32(z - y + (z < y ? m : 0));
	}

	constexpr u32 mul(u32 a, u32 b) const {
		return modulo(u64(a) * b);
	}

private:
	u32 m;
	u64 im;
};

struct barrett_64 {
	constexpr barrett_64(u64 m_): m(m_) {
		u128 im = u128(-1) / m;
		if (im * m + m == u128(0)) ++im;

		mh = im >> 64;
		ml = im & u64(-1);
	}

	constexpr u64 mod() const {
		return m;
	}

	constexpr u64 modulo(u128 x) const {
		u128 z = (x & u64(-1)) * ml;

		z = (x & u64(-1)) * mh + (x >> 64) * ml + (z >> 64);
		z = (x >> 64) * mh + (z >> 64);

		x -= z * m;
		return u64(x < m ? x : x - m);
	}

	constexpr u64 mul(u64 a, u64 b) const {
		return modulo(u128(a) * b);
	}

private:
	u64 m;
	u128 mh, ml;
};

template <typename U, is_unsigned_integral_t<U>* = nullptr>
using barrett = std::conditional<std::is_same_v<U, u32>, barrett_32, barrett_64>::type;

template <typename U, i32 id, is_unsigned_integral_t<U>* = nullptr>
struct dynamic_montgomery_modint_base {
	using mint = dynamic_montgomery_modint_base;

	using V = make_double<U>;
	using S = make_signed<U>;
	using T = make_signed<V>;

	static constexpr i32 W = std::numeric_limits<U>::digits;

	constexpr dynamic_montgomery_modint_base(): v(0) {}

	template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
	constexpr dynamic_montgomery_modint_base(T x):
		v(reduce(V(x % m + m) * n2)) {}

	constexpr static U reduce(V b) {
		return static_cast<U>((b + V(U(b) * U(-r)) * m) >> W);
	}

	constexpr static U get_r() {
		U p = m;
		while (m * p != 1) p *= U(2) - m * p;
		return p;
	}

	constexpr static void set_mod(U m_) {
		assert(m_ & 1 && m_ <= U(1) << (W - 2));

		m = m_;
		n2 = static_cast<U>(-V(m) % m);
		r = get_r();
	}

	constexpr U val() const {
		U p = reduce(v);
		return p >= m ? p - m : p;
	}

	constexpr static U mod() {
		return m;
	}

	constexpr mint inv() const {
		const auto &[f, s] = inv_gcd(val(), mod());
		assert(f == 1);

		return s;
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
	inline static U m, r, n2;
};

template <i32 id>
using dynamic_montgomery_modint_32 = dynamic_montgomery_modint_base<u32, id>;

template <i32 id>
using dynamic_montgomery_modint_64 = dynamic_montgomery_modint_base<u64, id>;

constexpr bool miller_rabin(u32 n) {
	if (n <= 2) return n == 2;
	if (n % 2 == 0) return false;

	u32 d = n - 1;
	while (d % 2 == 0) d >>= 1;

	using Z = dynamic_montgomery_modint_32<-1>;
	Z::set_mod(n);

	Z e = 1, r = n - 1;
	for (const u64 a : {2, 7, 61}) {
		if (a % n == 0) continue;

		u64 t = d;
		Z y = Z(a).pow(t);
		while (t != n - 1 && y != e && y != r) {
			y *= y;
			t <<= 1;
		}

		if (y != r && t % 2 == 0) return false;
	}

	return true;
}

constexpr bool miller_rabin(u64 n) {
	if (n <= 2) return n == 2;
	if (n % 2 == 0) return false;

	u64 d = n - 1;
	while (d % 2 == 0) d >>= 1;

	using Z = dynamic_montgomery_modint_64<-1>;
	Z::set_mod(n);

	Z e = 1, r = n - 1;
	for (const u64 a : {2, 325, 9375, 28178, 450775, 9780504, 1795265022}) {
		if (a % n == 0) continue;

		u64 t = d;
		Z y = Z(a).pow(t);
		while (t != n - 1 && y != e && y != r) {
			y *= y;
			t <<= 1;
		}

		if (y != r && t % 2 == 0) return false;
	}

	return true;
}

template <typename U, U m, is_unsigned_integral_t<U>* = nullptr>
constexpr bool is_prime_v = miller_rabin(m);

template <typename U, i32 id, is_unsigned_integral_t<U>* = nullptr>
struct dynamic_modint_base {
	using mint = dynamic_modint_base;

	constexpr dynamic_modint_base(): v(0) {}

	template <typename T, is_unsigned_integral_t<T>* = nullptr>
	constexpr dynamic_modint_base(T x): v(U(x % mod())) {}

	template <typename T, is_signed_integral_t<T>* = nullptr>
	constexpr dynamic_modint_base(T x) {
		using S = make_signed<U>;

		S u = S(x % S(mod()));
		if (u < 0) u += mod();

		v = u;
	}

	constexpr static void set_mod(U m) {
		bt = m;
	}

	constexpr static U mod() { 
		return bt.mod();
	}

	constexpr U val() const {
		return v;
	}

	constexpr mint operator-() const {
		mint r;
		r.v = (v == 0 ? 0 : mod() - v);
		return r;
	}

	constexpr mint inv() const {
		const auto &[f, s] = inv_gcd(v, mod());
		assert(f == 1);

		return s;
	}

	constexpr mint pow(u64 n) const {
		return binpow(*this, n);
	}

	constexpr mint& operator+=(const mint& rhs) & {
		v += rhs.val();
		if (v >= mod()) v -= mod();
		return *this;
	}

	constexpr mint& operator-=(const mint& rhs) & {
		v -= rhs.val();
		if (v >= mod()) v -= mod();
		return *this;
	}

	constexpr mint& operator*=(const mint& rhs) & {
		v = bt.mul(v, rhs.val());
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

	friend constexpr bool operator==(const mint& lhs, const mint& rhs) {
		return lhs.val() == rhs.val();
	}

	friend constexpr bool operator!=(const mint& lhs, const mint& rhs) {
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
using dynamic_modint_32 = dynamic_modint_base<u32, id>;

template <>
inline barrett<u32> dynamic_modint_32<-1>::bt = 998'244'353;

template <i32 id>
using dynamic_modint_64 = dynamic_modint_base<u64, id>;

template <>
inline barrett<u64> dynamic_modint_64<-1>::bt = (u64(1) << 61) - 1;

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
		if constexpr (is_prime) {
			return pow(mod() - 2);
		} else {
			const auto &[f, s] = inv_gcd(val(), mod());
			assert(f == 1);

			return s;
		}
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
	inline static constexpr bool is_prime = is_prime_v<U, m>;
};

template <u32 m>
using static_montgomery_modint_32 = static_montgomery_modint_base<u32, m>;

template <u64 m>
using static_montgomery_modint_64 = static_montgomery_modint_base<u64, m>;

template <typename U, U m, is_unsigned_integral_t<U>* = nullptr>
struct static_modint_base {
	using mint = static_modint_base;

	constexpr static_modint_base(): v(0) {}

	template <typename T, is_unsigned_integral_t<T>* = nullptr>
	constexpr static_modint_base(T x): v(U(x % mod())) {}

	template <typename T, is_signed_integral_t<T>* = nullptr>
	constexpr static_modint_base(T x) {
		using S = make_signed<U>;

		S u = S(x % S(mod()));
		if (u < 0) u += mod();

		v = u;
	}

	constexpr static U mod() { 
		return m;
	}

	constexpr U val() const {
		return v;
	}

	constexpr mint operator-() const {
		mint r;
		r.v = (v == 0 ? 0 : mod() - v);
		return r;
	}

	constexpr mint inv() const {
		if constexpr (is_prime) {
			return pow(mod() - 2);
		} else {
			const auto &[f, s] = inv_gcd(v, mod());
			assert(f == 1);

			return s;
		}
	}

	constexpr mint pow(u64 n) const {
		return binpow(*this, n);
	}

	constexpr mint& operator+=(const mint& rhs) & {
		v += rhs.val();
		if (v >= mod()) v -= mod();
		return *this;
	}

	constexpr mint& operator-=(const mint& rhs) & {
		v -= rhs.val();
		if (v >= mod()) v -= mod();
		return *this;
	}

	constexpr mint& operator*=(const mint& rhs) & {
		v = mul<mod()>(v, rhs.val());
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

	friend constexpr bool operator==(const mint& lhs, const mint& rhs) {
		return lhs.val() == rhs.val();
	}

	friend constexpr bool operator!=(const mint& lhs, const mint& rhs) {
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
	inline static constexpr bool is_prime = is_prime_v<U, m>;
};

template <u32 m>
using static_modint_32 = static_modint_base<u32, m>;

template <u64 m>
using static_modint_64 = static_modint_base<u64, m>;

using modint998244353 = static_modint_32<998'244'353>;
using modint1000000007 = static_modint_32<1'000'000'007>;

#endif // LIB_MODINT_HPP
