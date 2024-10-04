#ifndef LIB_MODINT_HPP
#define LIB_MODINT_HPP 1

#include <cassert>
#include <iostream>
#include <type_traits>
#include <utility>
#include <lib/prelude.hpp>

// @param m `1 <= m`
// @return x mod m
constexpr i64 safe_mod(i64 x, i64 m) {
	x %= m;
	if (x < 0) x += m;
	return x;
}

// Fast modular multiplication by barrett reduction
// Reference: https://en.wikipedia.org/wiki/Barrett_reduction
// NOTE: reconsider after Ice Lake
struct barrett {
	u32 _m;
	u64 im;

	// @param m `1 <= m`
	constexpr explicit barrett(u32 m) : _m(m), im((u64)(-1) / m + 1) {}

	// @return m
	constexpr u32 umod() const { return _m; }

	// @param a `0 <= a < m`
	// @param b `0 <= b < m`
	// @return `a * b % m`
	constexpr u32 mul(u32 a, u32 b) const {
		// [1] m = 1
		// a = b = im = 0, so okay

		// [2] m >= 2
		// im = ceil(2^64 / m)
		// -> im * m = 2^64 + r (0 <= r < m)
		// let z = a*b = c*m + d (0 <= c, d < m)
		// a*b * im = (c*m + d) * im = c*(im*m) + d*im = c*2^64 + c*r + d*im
		// c*r + d*im < m * m + m * im < m * m + 2^64 + m <= 2^64 + m * (m + 1) < 2^64 * 2
		// ((ab * im) >> 64) == c or c + 1
		u64 z = a;
		z *= b;
		u64 x = (u64)(((u128)(z) * im) >> 64);

		u64 y = x * _m;
		return (u32)(z - y + (z < y ? _m : 0));
	}
};

// @param n `0 <= n`
// @param m `1 <= m`
// @return `(x ** n) % m`
constexpr i64 pow_mod_constexpr(i64 x, i64 n, i32 m) {
	if (m == 1) return 0;

	u32 _m = (u32)(m);
	u64 r = 1;
	u64 y = safe_mod(x, m);

	while (n) {
		if (n & 1) r = (r * y) % _m;
		y = (y * y) % _m;
		n >>= 1;
	}

	return r;
}

// Reference:
// M. Forisek and J. Jancina,
// Fast Primality Testing for Integers That Fit into a Machine Word
// @param n `0 <= n`
constexpr bool is_prime_constexpr(i32 n) {
	if (n <= 1) return false;
	if (n == 2 || n == 7 || n == 61) return true;
	if (n % 2 == 0) return false;

	i64 d = n - 1;
	while (d % 2 == 0) d /= 2;

	constexpr i64 bases[3] = {2, 7, 61};
	for (const i64 a : bases) {
		i64 t = d;
		i64 y = pow_mod_constexpr(a, t, n);
		while (t != n - 1 && y != 1 && y != n - 1) {
			y = y * y % n;
			t <<= 1;
		}

		if (y != n - 1 && t % 2 == 0) return false;
	}

	return true;
}

template <i32 n> constexpr bool is_prime = is_prime_constexpr(n);

// @param b `1 <= b`
// @return pair(g, x) s.t. g = gcd(a, b), xa = g (mod b), 0 <= x < b/g
constexpr std::pair<i64, i64> inv_gcd(i64 a, i64 b) {
	a = safe_mod(a, b);
	if (a == 0) return {b, 0};

	// Contracts:
	// [1] s - m0 * a = 0 (mod b)
	// [2] t - m1 * a = 0 (mod b)
	// [3] s * |m1| + t * |m0| <= b
	i64 s = b, t = a;
	i64 m0 = 0, m1 = 1;

	while (t) {
		i64 u = s / t;
		s -= t * u;
		m0 -= m1 * u;  // |m1 * u| <= |m1| * s <= b

		// [3]:
		// (s - t * u) * |m1| + t * |m0 - m1 * u|
		// <= s * |m1| - t * u * |m1| + t * (|m0| + |m1| * u)
		// = s * |m1| + t * |m0| <= b

		auto tmp = s;
		s = t;
		t = tmp;
		tmp = m0;
		m0 = m1;
		m1 = tmp;
	}

	// by [3]: |m0| <= b/g
	// by g != b: |m0| < b/g
	if (m0 < 0) m0 += b / s;
	return {s, m0};
}

// Compile time primitive root
// @param m must be prime
// @return primitive root (and minimum in now)
constexpr i32 primitive_root_constexpr(i32 m) {
	if (m == 2) return 1;
	if (m == 167772161) return 3;
	if (m == 469762049) return 3;
	if (m == 754974721) return 11;
	if (m == 998244353) return 3;

	i32 divs[20] = {};
	divs[0] = 2;

	i32 x = (m - 1) / 2;
	while (x % 2 == 0) x /= 2;

	i32 cnt = 1;
	for (i32 i = 3; (i64)(i) * i <= x; i += 2) {
		if (x % i == 0) {
			divs[cnt++] = i;
			while (x % i == 0) {
				x /= i;
			}
		}
	}

	if (x > 1) {
		divs[cnt++] = x;
	}

	for (i32 g = 2;; g++) {
		bool ok = true;
		for (i32 i = 0; i < cnt; i++) {
			if (pow_mod_constexpr(g, (m - 1) / divs[i], m) == 1) {
				ok = false;
				break;
			}
		}

		if (ok) return g;
	}
}

template <i32 m> constexpr i32 primitive_root = primitive_root_constexpr(m);

// @param n `n < 2^32`
// @param m `1 <= m < 2^32`
// @return sum_{i=0}^{n-1} floor((ai + b) / m) (mod 2^64)
inline u64 floor_sum_unsigned(u64 n, u64 m, u64 a, u64 b) {
	u64 ans = 0;
	while (true) {
		if (a >= m) {
			ans += n * (n - 1) / 2 * (a / m);
			a %= m;
		}

		if (b >= m) {
			ans += n * (b / m);
			b %= m;
		}

		u64 y_max = a * n + b;
		if (y_max < m) break;
		// y_max < m * (n + 1)
		// floor(y_max / m) <= n
		n = (u64)(y_max / m);
		b = (u64)(y_max % m);
		std::swap(m, a);
	}

	return ans;
}

template <class T> using is_integral = typename std::is_integral<T>;

template <class T>
using is_signed_int =
    typename std::conditional<is_integral<T>::value && std::is_signed<T>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using is_unsigned_int =
    typename std::conditional<is_integral<T>::value &&
                                  std::is_unsigned<T>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using to_unsigned = typename std::conditional<is_signed_int<T>::value,
                                              std::make_unsigned<T>,
                                              std::common_type<T>>::type;

template <class T>
using is_signed_int_t = std::enable_if_t<is_signed_int<T>::value>;

template <class T>
using is_unsigned_int_t = std::enable_if_t<is_unsigned_int<T>::value>;

template <class T> using to_unsigned_t = typename to_unsigned<T>::type;

struct modint_base {};
struct static_modint_base : modint_base {};

template <class T> using is_modint = std::is_base_of<modint_base, T>;
template <class T> using is_modint_t = std::enable_if_t<is_modint<T>::value>;

template <i32 m, std::enable_if_t<(1 <= m)>* = nullptr>
struct static_modint : static_modint_base {
	using mint = static_modint;

public:
	static constexpr i32 mod() { return m; }

	static mint raw(i32 v) {
		mint x;
		x._v = v;
		return x;
	}

	constexpr static_modint() : _v(0) {}

	template <class T, is_signed_int_t<T>* = nullptr>
	constexpr static_modint(T v) {
		i64 x = (i64)(v % (i64)(umod()));
		if (x < 0) x += umod();
		_v = (u32)(x);
	}

	template <class T, is_unsigned_int_t<T>* = nullptr>
	constexpr static_modint(T v) {
		_v = (u32)(v % umod());
	}

	u32 val() const { return _v; }

	mint& operator++() {
		_v++;
		if (_v == umod()) _v = 0;
		return *this;
	}

	mint& operator--() {
		if (_v == 0) _v = umod();
		_v--;
		return *this;
	}

	mint operator++(i32) {
		mint result = *this;
		++*this;
		return result;
	}

	mint operator--(i32) {
		mint result = *this;
		--*this;
		return result;
	}

	mint& operator+=(const mint& rhs) {
		_v += rhs._v;
		if (_v >= umod()) _v -= umod();
		return *this;
	}

	mint& operator-=(const mint& rhs) {
		_v -= rhs._v;
		if (_v >= umod()) _v += umod();
		return *this;
	}

	mint& operator*=(const mint& rhs) {
		u64 z = _v;
		z *= rhs._v;
		_v = (u32)(z % umod());
		return *this;
	}

	mint& operator/=(const mint& rhs) { return *this = *this * rhs.inv(); }

	mint operator+() const { return *this; }
	mint operator-() const { return mint() - *this; }

	mint pow(i64 n) const {
		assert(0 <= n);	

		mint x = *this, r = 1;
		while (n) {
			if (n & 1) r *= x;
			x *= x;
			n >>= 1;
		}

		return r;
	}

	mint inv() const {
		if (prime) {
			assert(_v);
			return pow(umod() - 2);
		} else {
			auto eg = inv_gcd(_v, m);
			assert(eg.first == 1);
			return eg.second;
		}
	}

	friend mint operator+(const mint& lhs, const mint& rhs) {
		return mint(lhs) += rhs;
	}

	friend mint operator-(const mint& lhs, const mint& rhs) {
		return mint(lhs) -= rhs;
	}

	friend mint operator*(const mint& lhs, const mint& rhs) {
		return mint(lhs) *= rhs;
	}

	friend mint operator/(const mint& lhs, const mint& rhs) {
		return mint(lhs) /= rhs;
	}

	friend bool operator==(const mint& lhs, const mint& rhs) {
		return lhs._v == rhs._v;
	}

	friend bool operator!=(const mint& lhs, const mint& rhs) {
		return lhs._v != rhs._v;
	}

	friend std::ostream& operator<<(std::ostream& os, const mint& rhs) {
		return os << rhs.val();
	}

	friend std::istream& operator>>(std::istream& is, mint& rhs) {
		i64 v;
		is >> v;

		rhs = mint(v);
		return is;
	}

	u32 _v;
	static constexpr u32 umod() { return m; }
	static constexpr bool prime = is_prime<m>;
};

template <i32 id> 
struct dynamic_modint : modint_base {
	using mint = dynamic_modint;

public:
	static i32 mod() { return (i32)(bt.umod()); }

	static void set_mod(i32 m) {
		assert(1 <= m);
		bt = barrett(m);
	}

	static mint raw(i32 v) {
		mint x;
		x._v = v;
		return x;
	}

	dynamic_modint() : _v(0) {}

	template <class T, is_signed_int_t<T>* = nullptr>
	dynamic_modint(T v) {
		i64 x = (i64)(v % (i64)(mod()));
		if (x < 0) x += mod();
		_v = (u32)(x);
	}

	template <class T, is_unsigned_int_t<T>* = nullptr>
	dynamic_modint(T v) {
		_v = (u32)(v % mod());
	}

	u32 val() const { return _v; }

	mint& operator++() {
		_v++;
		if (_v == umod()) _v = 0;
		return *this;
	}

	mint& operator--() {
		if (_v == 0) _v = umod();
		_v--;
		return *this;
	}

	mint operator++(i32) {
		mint result = *this;
		++*this;
		return result;
	}

	mint operator--(i32) {
		mint result = *this;
		--*this;
		return result;
	}

	mint& operator+=(const mint& rhs) {
		_v += rhs._v;
		if (_v >= umod()) _v -= umod();
		return *this;
	}

	mint& operator-=(const mint& rhs) {
		_v += mod() - rhs._v;
		if (_v >= umod()) _v -= umod();
		return *this;
	}

	mint& operator*=(const mint& rhs) {
		_v = bt.mul(_v, rhs._v);
		return *this;
	}

	mint& operator/=(const mint& rhs) { return *this = *this * rhs.inv(); }

	mint operator+() const { return *this; }
	mint operator-() const { return mint() - *this; }

	mint pow(i64 n) const {
		assert(0 <= n);

		mint x = *this, r = 1;
		while (n) {
			if (n & 1) r *= x;
			x *= x;
			n >>= 1;
		}

		return r;
	}

	mint inv() const {
		auto eg = inv_gcd(_v, mod());
		assert(eg.first == 1);
		return eg.second;
	}

	friend mint operator+(const mint& lhs, const mint& rhs) {
		return mint(lhs) += rhs;
	}

	friend mint operator-(const mint& lhs, const mint& rhs) {
		return mint(lhs) -= rhs;
	}

	friend mint operator*(const mint& lhs, const mint& rhs) {
		return mint(lhs) *= rhs;
	}

	friend mint operator/(const mint& lhs, const mint& rhs) {
		return mint(lhs) /= rhs;
	}

	friend bool operator==(const mint& lhs, const mint& rhs) {
		return lhs._v == rhs._v;
	}

	friend bool operator!=(const mint& lhs, const mint& rhs) {
		return lhs._v != rhs._v;
	}

	friend std::ostream& operator<<(std::ostream& os, const mint& rhs) {
		return os << rhs.val();
	}

	friend std::istream& operator>>(std::istream& is, mint& rhs) {
		i64 v;
		is >> v;

		rhs = mint(v);
		return is;
	}

private:
	u32 _v;
	static barrett bt;
	static u32 umod() { return bt.umod(); }
};

template <i32 id> barrett dynamic_modint<id>::bt(998244353);

using modint998244353 = static_modint<998244353>;
using modint1000000007 = static_modint<1000000007>;
using modint = dynamic_modint<-1>;

template <class T>
using is_static_modint = std::is_base_of<static_modint_base, T>;

template <class T>
using is_static_modint_t = std::enable_if_t<is_static_modint<T>::value>;

template <class> struct is_dynamic_modint : public std::false_type {};

template <i32 id>
struct is_dynamic_modint<dynamic_modint<id>> : public std::true_type {};

template <class T>
using is_dynamic_modint_t = std::enable_if_t<is_dynamic_modint<T>::value>;

#endif // LIB_MODINT_HPP
