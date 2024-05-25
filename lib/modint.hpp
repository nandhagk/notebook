#ifndef LIB_MODINT_HPP
#define LIB_MODINT_HPP 1

#include <iostream>
#include <lib/type_traits.hpp>

template<class T>
constexpr T power(T a, i64 b) {
	T res = 1;

	for (; b; b >>= 1, a *= a) {
		if (b & 1) res *= a;
	}

	return res;
}

constexpr i64 mul(i64 a, i64 b, i64 p) {
	i64 res = a * b - i64(1.L * a * b / p) * p;
	res %= p;
	if (res < 0) {
		res += p;
	}

	return res;
}

template<i64 P>
struct ModInt {
public:
	constexpr ModInt() : x(0) {}
	constexpr ModInt(i64 x_) : x(norm(x_ % getMod())) {}

	static i64 Mod;

	constexpr static i64 getMod() {
		if (P > 0) {
			return P;
		} else {
			return Mod;
		}
	}

	constexpr static void setMod(i64 Mod_) {
		Mod = Mod_;
	}

	constexpr i64 norm(i64 x_) const {
		if (x_ < 0) {
			x_ += getMod();
		}

		if (x_ >= getMod()) {
			x_ -= getMod();
		}

		return x_;
	}

	constexpr i64 val() const {
		return x;
	}

	constexpr ModInt operator-() const {
		ModInt res;
		res.x = norm(getMod() - x);
		return res;
	}

	constexpr ModInt pow(i64 n) const {
		return power(*this, n);
	}

	constexpr ModInt inv() const {
		return power(*this, getMod() - 2);
	}

	constexpr ModInt &operator*=(ModInt rhs) & {
		if (getMod() < (1ULL << 31)) {
			x = x * rhs.x % int(getMod());
		} else {
			x = mul(x, rhs.x, getMod());
		}

		return *this;
	}

	constexpr ModInt &operator+=(ModInt rhs) & {
		x = norm(x + rhs.x);
		return *this;
	}

	constexpr ModInt &operator-=(ModInt rhs) & {
		x = norm(x - rhs.x);
		return *this;
	}

	constexpr ModInt &operator/=(ModInt rhs) & {
		return *this *= rhs.inv();
	}

	friend constexpr ModInt operator*(ModInt lhs, ModInt rhs) {
		ModInt res = lhs;
		res *= rhs;
		return res;
	}

	friend constexpr ModInt operator+(ModInt lhs, ModInt rhs) {
		ModInt res = lhs;
		res += rhs;
		return res;
	}

	friend constexpr ModInt operator-(ModInt lhs, ModInt rhs) {
		ModInt res = lhs;
		res -= rhs;
		return res;
	}

	friend constexpr ModInt operator/(ModInt lhs, ModInt rhs) {
		ModInt res = lhs;
		res /= rhs;
		return res;
	}

	friend constexpr std::istream &operator>>(std::istream &is, ModInt &a) {
		i64 v;
		is >> v;
		a = ModInt(v);
		return is;
	}

	friend constexpr std::ostream &operator<<(std::ostream &os, const ModInt &a) {
		return os << a.val();
	}

	friend constexpr bool operator==(ModInt lhs, ModInt rhs) {
		return lhs.val() == rhs.val();
	}

	friend constexpr bool operator!=(ModInt lhs, ModInt rhs) {
		return lhs.val() != rhs.val();
	}

	friend constexpr bool operator<(ModInt lhs, ModInt rhs) {
		return lhs.val() < rhs.val();
	}

private:
	i64 x;
};

using ModInt998244353 = ModInt<998'244'353>;
using ModInt1000000007 = ModInt<1'000'000'007>;

#endif // LIB_MODINT_HPP
