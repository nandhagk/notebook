#ifndef LIB_MILLER_RABIN_HPP
#define LIB_MILLER_RABIN_HPP 1

#include <lib/prelude.hpp>
#include <lib/arbitrary_montgomery_modint.hpp>

constexpr bool miller_rabin(u32 n) {
	if (n <= 2) return n == 2;
	if (n % 2 == 0) return false;

	u32 d = n - 1;
	while (d % 2 == 0) d >>= 1;

	using Z = arbitrary_montgomery_modint_32<-1>;
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

	using Z = arbitrary_montgomery_modint_64<-1>;
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

inline bool is_prime(u64 n) {
        if (n < (1 << 30)) {
                return miller_rabin(static_cast<u32>(n));
        } else {
                return miller_rabin(n);
        }
}

#endif // LIB_MILLER_RABIN_HPP
