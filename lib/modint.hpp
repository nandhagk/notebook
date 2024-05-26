#ifndef LIB_MODINT_HPP
#define LIB_MODINT_HPP 1

#include <iostream>
#include <cassert>
#include <lib/prelude.hpp>
#include <lib/math.hpp>

template <i32 m, std::enable_if_t<(1 <= m)>* = nullptr>
struct StaticModInt {
public:
	using ModInt = StaticModInt;

	static constexpr i32 mod() {
		return m;
	}

	static ModInt raw(i32 v) {
		ModInt x;
		x.v = v;
		return x;
	}

	StaticModInt() : v(0) {}

	template <class T, is_signed_int_t<T>* = nullptr>
	StaticModInt(T v_) {
		i64 x = static_cast<i64>(v_ % static_cast<i64>(umod()));
		if (x < 0) x += umod();
		v = static_cast<u32>(x);
	}

	template <class T, is_unsigned_int_t<T>* = nullptr>
	StaticModInt(T v_) {
		v = static_cast<u32>(v_ % umod());
	}

	u32 val() const {
		return v;
	}

	ModInt& operator++() {
		v++;
		if (v == umod()) v = 0;
		return *this;
	}

	ModInt& operator--() {
		if (v == 0) v = umod();
		v--;
		return *this;
	}

	ModInt operator++(int) {
		ModInt result = *this;
		++*this;
		return result;
	}

	ModInt operator--(int) {
		ModInt result = *this;
		--*this;
		return result;
	}

	ModInt& operator+=(const ModInt& rhs) {
		v += rhs.v;
		if (v >= umod()) v -= umod();
		return *this;
	}

	ModInt& operator-=(const ModInt& rhs) {
		v -= rhs.v;
		if (v >= umod()) v += umod();
		return *this;
	}

	ModInt& operator*=(const ModInt& rhs) {
		u64 z = v;
		z *= rhs.v;
		v = static_cast<u32>(z % umod());
		return *this;
	}

	ModInt& operator/=(const ModInt& rhs) {
		return *this = *this * rhs.inv();
	}

	ModInt operator+() const {
		return *this;
	}

	ModInt operator-() const {
		return ModInt() - *this;
	}

	ModInt pow(i64 n) const {
		assert(0 <= n);

		ModInt r = 1;
		for (ModInt x = *this; n; x *= x, n >>= 1) {
			if (n & 1) r *= x;
		}

		return r;
	}

	ModInt inv() const {
		if (prime) {
			assert(v);

			return pow(umod() - 2);
		} else {
			const auto &[f, s] = inv_gcd(v, m);
			assert(f == 1);

			return s;
		}
	}

	friend ModInt operator+(const ModInt& lhs, const ModInt& rhs) {
		return ModInt(lhs) += rhs;
	}

	friend ModInt operator-(const ModInt& lhs, const ModInt& rhs) {
		return ModInt(lhs) -= rhs;
	}

	friend ModInt operator*(const ModInt& lhs, const ModInt& rhs) {
		return ModInt(lhs) *= rhs;
	}

	friend ModInt operator/(const ModInt& lhs, const ModInt& rhs) {
		return ModInt(lhs) /= rhs;
	}

	friend bool operator==(const ModInt& lhs, const ModInt& rhs) {
		return lhs.v == rhs.v;
	}

	friend bool operator!=(const ModInt& lhs, const ModInt& rhs) {
		return lhs.v != rhs.v;
	}

	friend std::istream &operator>>(std::istream &is, ModInt &rhs) {
		i64 v_;
		is >> v_;
		rhs = ModInt(v_);
		return is;
	}

	friend std::ostream &operator<<(std::ostream &os, const ModInt &rhs) {
		return os << rhs.v;
	}

private:
	u32 v;
	static constexpr bool prime = is_prime_v<m>;

	static constexpr u32 umod() {
		return m;
	}
};

template <i32 id>
struct DynamicModInt {
public:

	using ModInt = DynamicModInt;

	static i32 mod() {
		return static_cast<i32>(bt.umod());
	}

	static void set_mod(i32 m) {
		assert(1 <= m);

		bt = Barrett(m);
	}

	static ModInt raw(i32 v) {
		ModInt x;
		x.v = v;
		return x;
	}

	DynamicModInt() : v(0) {}

	template <class T, is_signed_int_t<T>* = nullptr>
	DynamicModInt(T v_) {
		i64 x = static_cast<i64>(v_ % static_cast<i64>(mod()));
		if (x < 0) x += mod();
		v = static_cast<u32>(x);
	}

	template <class T, is_unsigned_int_t<T>* = nullptr>
	DynamicModInt(T v_) {
		v = static_cast<u32>(v_ % mod());
	}

	u32 val() const {
		return v;
	}

	ModInt& operator++() {
		v++;
		if (v == umod()) v = 0;
		return *this;
	}

	ModInt& operator--() {
		if (v == 0) v = umod();
		v--;
		return *this;
	}

	ModInt operator++(int) {
		ModInt result = *this;
		++*this;
		return result;
	}

	ModInt operator--(int) {
		ModInt result = *this;
		--*this;
		return result;
	}

	ModInt& operator+=(const ModInt& rhs) {
		v += rhs.v;
		if (v >= umod()) v -= umod();
		return *this;
	}

	ModInt& operator-=(const ModInt& rhs) {
		v += mod() - rhs.v;
		if (v >= umod()) v -= umod();
		return *this;
	}

	ModInt& operator*=(const ModInt& rhs) {
		v = bt.mul(v, rhs.v);
		return *this;
	}

	ModInt& operator/=(const ModInt& rhs) {
		return *this = *this * rhs.inv();
	}

	ModInt operator+() const {
		return *this;
	}

	ModInt operator-() const {
		return ModInt() - *this;
	}

	ModInt pow(i64 n) const {
		assert(0 <= n);

		ModInt r = 1;
		for (ModInt x = *this; n; x *= x, n >>= 1) {
			if (n & 1) r *= x;
		}

		return r;
	}

	ModInt inv() const {
		const auto &[f, s] = inv_gcd(v, mod());
		assert(f == 1);

		return s;
	}

	friend ModInt operator+(const ModInt& lhs, const ModInt& rhs) {
		return ModInt(lhs) += rhs;
	}

	friend ModInt operator-(const ModInt& lhs, const ModInt& rhs) {
		return ModInt(lhs) -= rhs;
	}

	friend ModInt operator*(const ModInt& lhs, const ModInt& rhs) {
		return ModInt(lhs) *= rhs;
	}

	friend ModInt operator/(const ModInt& lhs, const ModInt& rhs) {
		return ModInt(lhs) /= rhs;
	}

	friend bool operator==(const ModInt& lhs, const ModInt& rhs) {
		return lhs.v == rhs.v;
	}

	friend bool operator!=(const ModInt& lhs, const ModInt& rhs) {
		return lhs.v != rhs.v;
	}

	friend std::istream &operator>>(std::istream &is, ModInt &rhs) {
		i64 v_;
		is >> v_;
		rhs = ModInt(v_);
		return is;
	}

	friend std::ostream &operator<<(std::ostream &os, const ModInt &rhs) {
		return os << rhs.v;
	}

private:
	u32 v;
	static Barrett bt;

	static u32 umod() {
		return bt.umod();
	}
};

template <i32 id>
Barrett DynamicModInt<id>::bt(998244353);

using ModInt998244353 = StaticModInt<998244353>;
using ModInt1000000007 = StaticModInt<1000000007>;
using ModInt = DynamicModInt<-1>;

#endif // LIB_MODINT_HPP
