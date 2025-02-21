#ifndef LIB_CONVOLUTION_NAIVE_HPP
#define LIB_CONVOLUTION_NAIVE_HPP 1

#include <vector>

#include <lib/prelude.hpp>

template <typename T>
inline std::vector<T> convolution_naive(const std::vector<T> &a, const std::vector<T> &b) {
    const i32 n = static_cast<i32>(a.size());
    const i32 m = static_cast<i32>(b.size());

    if (n > m) return convolution_naive(b, a);
    if (n == 0) return {};

    std::vector<T> c(n + m - 1);
    for (i32 i = 0; i < n; ++i)
        for (i32 j = 0; j < m; ++j) c[i + j] += a[i] * b[j];

    return c;
}

#endif // LIB_CONVOLUTION_NAIVE_HPP
