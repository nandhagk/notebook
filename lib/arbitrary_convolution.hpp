#ifndef LIB_ARBITRARY_CONVOLUTION_HPP
#define LIB_ARBITRARY_CONVOLUTION_HPP 1

#include <vector>

#include <lib/prelude.hpp>
#include <lib/static_montgomery_modint.hpp>
#include <lib/convolution.hpp>

template <typename Z>
std::vector<Z> arbitrary_convolution(std::vector<Z> l, std::vector<Z> r) {
        static constexpr i64 MOD1 = 754974721;  // 2^24
        static constexpr i64 MOD2 = 167772161;  // 2^25
        static constexpr i64 MOD3 = 469762049;  // 2^26

        static constexpr i64 M1M2 = MOD1 * MOD2;
        static constexpr i64 INV_M1_MOD2 = inv(MOD1, MOD2);
        static constexpr i64 INV_M1M2_MOD3 = inv(M1M2, MOD3);

        using Z1 = static_montgomery_modint_32<MOD1>;
        using Z2 = static_montgomery_modint_32<MOD2>;
        using Z3 = static_montgomery_modint_32<MOD3>;

        const i32 n = static_cast<i32>(l.size());
        const i32 m = static_cast<i32>(r.size());

        std::vector<u32> a2(n), b2(m);
        for (i32 i = 0; i < n; ++i) a2[i] = l[i].val();
        for (i32 i = 0; i < m; ++i) b2[i] = r[i].val();

        const auto c1 = convolution(std::vector<Z1>(a2.begin(), a2.end()), std::vector<Z1>(b2.begin(), b2.end()));
        const auto c2 = convolution(std::vector<Z2>(a2.begin(), a2.end()), std::vector<Z2>(b2.begin(), b2.end()));
        const auto c3 = convolution(std::vector<Z3>(a2.begin(), a2.end()), std::vector<Z3>(b2.begin(), b2.end()));
        const i64 m1m2 = Z(M1M2).val();

        std::vector<Z> c(n + m - 1);
        for (i32 i = 0; i < n + m - 1; ++i) {
                i64 x1 = c1[i].val();
                i64 y1 = c2[i].val();
                i64 z1 = c3[i].val();

                i64 x2 = (Z2(y1 - x1) * INV_M1_MOD2).val();
                i64 x3 = (Z3(z1 - x1 - x2 * MOD1) * INV_M1M2_MOD3).val();

                c[i] = x1 + x2 * MOD1 + x3 * m1m2;
        }

        return c;
}

#endif // LIB_ARBITRARY_CONVOLUTION_HPP
