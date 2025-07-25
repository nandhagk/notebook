#ifndef LIB_NTT_HPP
#define LIB_NTT_HPP 1

#include <array>
#include <cassert>
#include <vector>

#include <lib/bits.hpp>
#include <lib/prelude.hpp>

template <typename Z>
void ntt(std::vector<Z> &a, bool inv) {
    static std::array<Z, 30> dw{}, iw{};
    if (dw[0] == 0) {
        Z root = 2;
        while (root.pow((Z::mod() - 1) / 2) == 1) root += 1;

        for (i32 i = 0; i < 30; ++i) {
            dw[i] = -root.pow((Z::mod() - 1) >> (i + 2));
            iw[i] = dw[i].inv();
        }
    }

    const i32 n = static_cast<i32>(a.size());
    assert((n & (n - 1)) == 0);

    if (!inv) {
        for (i32 m = n; m >>= 1;) {
            Z w = 1;
            for (i32 s{}, k{}; s < n; s += 2 * m) {
                for (i32 i = s, j = s + m; i < s + m; ++i, ++j) {
                    Z x = a[i], y = a[j] * w;
                    a[i] = x + y;
                    a[j] = x - y;
                }

                w *= dw[lowbit(++k)];
            }
        }
    } else {
        for (i32 m = 1; m < n; m <<= 1) {
            Z w = 1;
            for (i32 s{}, k{}; s < n; s += 2 * m) {
                for (i32 i = s, j = s + m; i < s + m; ++i, ++j) {
                    Z x = a[i], y = a[j];
                    a[i] = x + y;
                    a[j] = (x - y) * w;
                }

                w *= iw[lowbit(++k)];
            }
        }

        Z r = Z(n).inv();
        for (auto &&e : a) e *= r;
    }
}

#endif // LIB_NTT_HPP
