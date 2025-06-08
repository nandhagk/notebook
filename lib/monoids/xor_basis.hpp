#ifndef LIB_MONOID_XOR_BASIS_HPP
#define LIB_MONOID_XOR_BASIS_HPP 1

#include <array>
#include <limits>

#include <lib/numeric_traits.hpp>
#include <lib/prelude.hpp>

template <typename T, i32 W = std::numeric_limits<T>::digits, is_integral_t<T> * = nullptr>
struct monoid_xor_basis {
    using X = std::pair<i32, std::array<T, W>>;
    using ValueT = X;

    static constexpr X op(const X &x, const X &y) {
        X z = x;

        const auto &[sz, b] = y;
        for (i32 i = 0; i < W; ++i) insert(z, b[i]);

        return z;
    }

    static constexpr X unit() {
        return {0, std::array<T, W>{}};
    }

    static constexpr X from_element(T t) {
        X x = unit();
        insert(x, t);

        return x;
    }

    static constexpr void insert(X &x, T t) {
        auto &[sz, b] = x;
        for (i32 i = 0; i < W; ++i) {
            if (!((t >> i) & 1)) continue;

            if (!b[i]) {
                b[i] = t;
                ++sz;

                return;
            }

            t ^= b[i];
        }
    }

    static constexpr bool belongs(const X &x, T t) {
        const auto &[sz, b] = x;

        for (i32 i = 0; i < W; ++i) {
            if (!((t >> i) & 1)) continue;

            if (!b[i]) return false;
            t ^= b[i];
        }

        return true;
    }

    static constexpr bool commutative = true;
};

#endif // LIB_MONOID_XOR_BASIS_HPP
