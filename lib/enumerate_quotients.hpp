#ifndef LIB_ENUMERATE_QUOTIENTS_HPP
#define LIB_ENUMERATE_QUOTIENTS_HPP 1

#include <cmath>
#include <vector>

#include <lib/numeric_traits.hpp>
#include <lib/prelude.hpp>

template <typename U, is_unsigned_integral_t<U> * = nullptr>
std::vector<U> enumerate_quotients(U n) {
    U p = static_cast<U>(std::sqrt(n));

    if (p * p + p <= n) ++p;
    U m = n / p;

    std::vector<U> qs(p + m - 1);
    for (U i = 0; i < m; ++i) qs[i] = i + 1;
    for (U i = 0; i < p - 1; ++i) qs[i + m] = n / (p - 1 - i);

    return qs;
}

#endif // LIB_ENUMERATE_QUOTIENTS_HPP
