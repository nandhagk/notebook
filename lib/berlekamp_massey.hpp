#ifndef LIB_BERLEKAMP_MASSEY_HPP
#define LIB_BERLEKAMP_MASSEY_HPP 1

#include <vector>

#include <lib/prelude.hpp>

template <typename Z>
inline std::vector<Z> berlekamp_massey(const std::vector<Z> &a) {
    std::vector<Z> s(a), c{1}, b{1};

    const i32 n = static_cast<i32>(a.size());

    i32 m = 1;
    Z w = 1;

    for (i32 i = 0; i < n; ++i) {
        Z d = 0;
        for (i32 j = 0; j < static_cast<i32>(c.size()); ++j) d += c[j] * s[i - j];

        if (d == 0)
            ++m;
        else if (2 * (static_cast<i32>(c.size()) - 1) <= i) {
            auto t = c;

            Z coef = d / w;

            c.resize(std::max(c.size(), b.size() + m));
            for (i32 j = 0; j < static_cast<i32>(b.size()); ++j) c[j + m] -= coef * b[j];

            b = t;
            w = d;
            m = 1;
        } else {
            Z coef = d / w;

            c.resize(std::max(c.size(), b.size() + m));
            for (i32 j = 0; j < static_cast<i32>(b.size()); ++j) c[j + m] -= coef * b[j];

            ++m;
        }
    }

    c.erase(c.begin());
    for (i32 i = 0; i < static_cast<i32>(c.size()); ++i) c[i] *= -1;

    return c;
}

#endif // LIB_BERLEKAMP_MASSEY_HPP
