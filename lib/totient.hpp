#ifndef LIB_TOTIENT_HPP
#define LIB_TOTIENT_HPP 1

#include <lib/prelude.hpp>
#include <lib/math.hpp>
#include <lib/factorize.hpp>

template <typename U, is_unsigned_integral_t<U>* = nullptr>
inline U totient(U n) {
	U r = n;
	for (const auto &[p, _] : factorize(n)) r -= r / p;

	return r;
}

#endif // LIB_TOTIENT_HPP
