#ifndef LIB_CONVOLUTION_KARATSUBA_HPP
#define LIB_CONVOLUTION_KARATSUBA_HPP 1

#include <cassert>
#include <vector>

#include <lib/convolution_naive.hpp>
#include <lib/prelude.hpp>

template <typename T, i32 THRESHOLD = 30>
inline std::vector<T> convolution_karatsuba(const std::vector<T> &a, const std::vector<T> &b) {
    const i32 p = static_cast<i32>(a.size());
    const i32 q = static_cast<i32>(b.size());

    if (std::min(p, q) <= THRESHOLD) return convolution_naive(a, b);

    const i32 n = std::max(p, q);
    const i32 m = (n + 1) / 2;

    std::vector<T> u, v, w, x;

    if (p < m) {
        u = a;
    } else {
        u = {a.begin(), a.begin() + m};
        v = {a.begin() + m, a.end()};
    }

    if (q < m) {
        w = b;
    } else {
        w = {b.begin(), b.begin() + m};
        x = {b.begin() + m, b.end()};
    }

    const auto c = convolution_karatsuba(u, w);
    const auto d = convolution_karatsuba(v, x);

    for (i32 i = 0; i < static_cast<i32>(v.size()); ++i) u[i] += v[i];
    for (i32 i = 0; i < static_cast<i32>(x.size()); ++i) w[i] += x[i];

    auto e = convolution_karatsuba(u, w);

    std::vector<T> f(p + q - 1);
    for (i32 i = 0; i < static_cast<i32>(c.size()); ++i) {
        f[i] += c[i];
        e[i] -= c[i];
    }

    for (i32 i = 0; i < static_cast<i32>(d.size()); ++i) {
        f[2 * m + i] += d[i];
        e[i] -= d[i];
    }

    if (e.back() == T(0)) e.pop_back();
    for (i32 i = 0; i < static_cast<i32>(e.size()); ++i)
        if (e[i] != T(0)) f[m + i] += e[i];

    return f;
}

#endif // LIB_CONVOLUTION_KARATSUBA_HPP
