#ifndef LIB_TOTIENT_HPP
#define LIB_TOTIENT_HPP 1

#include <algorithm>

#include <lib/prelude.hpp>
#include <lib/math.hpp>
#include <lib/factorize.hpp>

template <typename U, is_unsigned_integral_t<U>* = nullptr>
inline U totient(U n) {
	auto ps = factorize(n);
	ps.erase(std::unique(ps.begin(), ps.end()), ps.end());

	U r = n;
	for (const U p : ps) r -= r / p;

	return r;
}

#endif // LIB_TOTIENT_HPP
