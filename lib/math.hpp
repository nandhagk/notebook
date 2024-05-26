#ifndef LIB_MATH_HPP
#define LIB_MATH_HPP 1

#include <utility>
#include <vector>
#include <cassert>
#include <lib/prelude.hpp>

constexpr i64 safe_mod(i64 x, i64 m) {
	x %= m;
	if (x < 0) x += m;
	return x;
}

struct Barrett {
public:
	explicit Barrett(u32 m_) : m(m_), im(static_cast<u64>(-1) / m + 1) {}

	u32 umod() const {
		return m;
	}

	u32 mul(u32 a, u32 b) const {
		const u64 z = static_cast<u64>(a) * b;
		const u64 x = static_cast<u64>((static_cast<u128>(z) * im) >> 64);
		const u64 y = x * m;
		return static_cast<u32>(z - y + (z < y ? m : 0));
	}

private:
	u32 m;
	u64 im;
};

constexpr i64 pow_mod(i64 x, i64 n, i32 m) {
	if (m == 1) return 0;

	const u32 _m = static_cast<u32>(m);
	u64 r = 1;

	for (u64 y = safe_mod(x, m); n; y = (y * y) % _m, n >>= 1) {
		if (n & 1) r = (r * y) % _m;
	}

	return r;
}

constexpr bool is_prime(i32 n) {
	if (n <= 1) return false;
	if (n == 2 || n == 7 || n == 61) return true;
	if (n % 2 == 0) return false;

	i64 d = n - 1;
	while (d % 2 == 0) d /= 2;

	constexpr i64 bases[3] = {2, 7, 61};
	for (const i64 a : bases) {
		i64 t = d;
		i64 y = pow_mod(a, t, n);

		while (t != n - 1 && y != 1 && y != n - 1) {
			y = y * y % n;
			t <<= 1;
		}

		if (y != n - 1 && t % 2 == 0) {
			return false;
		}
	}

	return true;
}

template <i32 n>
constexpr bool is_prime_v = is_prime(n);

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

std::pair<i64, i64> crt(const std::vector<i64>& r, const std::vector<i64>& m) {
	assert(r.size() == m.size());

	i32 n = static_cast<i32>(r.size());
	i64 r0 = 0, m0 = 1;

	for (i32 i = 0; i < n; i++) {
		assert(1 <= m[i]);

		i64 r1 = safe_mod(r[i], m[i]), m1 = m[i];
		if (m0 < m1) {
			std::swap(r0, r1);
			std::swap(m0, m1);
		}

		if (m0 % m1 == 0) {
			if (r0 % m1 != r1) return {0, 0};
			continue;
		}

		auto [g, im] = inv_gcd(m0, m1);

		i64 u1 = (m1 / g);
		if ((r1 - r0) % g) return {0, 0};

		i64 x = (r1 - r0) / g % u1 * im % u1;

		r0 += x * m0;
		m0 *= u1;
		if (r0 < 0) r0 += m0;
	}

	return {r0, m0};
}


template <class T>
using is_integral = typename std::is_integral<T>;

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

template <class T>
using to_unsigned_t = typename to_unsigned<T>::type;

#endif // LIB_MATH_HPP
