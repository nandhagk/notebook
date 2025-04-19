#ifndef LIB_STATIC_PALINDROME_HPP
#define LIB_STATIC_PALINDROME_HPP 1

#include <vector>

#include <lib/prelude.hpp>
#include <lib/manacher.hpp>
#include <lib/monoids/add.hpp>
#include <lib/static_product.hpp>
#include <lib/wavelet_matrix_product.hpp>

struct static_palindrome {
    i32 n;
    static_product<monoid_add<i64>> as;
    wavelet_matrix_product<i64, static_product<monoid_add<i64>>> wml1, wmr1, wml2, wmr2;

    static_palindrome() {}

    explicit static_palindrome(const std::string &s) {
        build(s);
    }

    void build(const std::string &s) {
        n = static_cast<i32>(s.size());
        const auto [odd, even] = manacher(s);

        std::vector<i64> l1(n), r1(n), l2(n), r2(n);
        for (i32 i = 0; i < n; ++i) {
            l1[i] = odd[i] - i;
            r1[i] = odd[i] + i;
            l2[i] = even[i] - i;
            r2[i] = even[i] + i;
        }

        as.build(n, [&](i32 i) { return i; });
        wml1.build(l1, l1);
        wmr1.build(r1, r1);
        wml2.build(l2, l2);
        wmr2.build(r2, r2);
    }

    i64 prod(i32 l, i32 r) const {
        assert(0 <= l && l <= r && r <= n);

        if (l == r) return 0;

        --r;
        const i32 m1 = (l + r) / 2;
        const auto [s1, x1] = wml1.count(l, m1 + 1, 1 - l);
        const auto [t1, y1] = wmr1.count(m1 + 1, r + 1, r + 1);

        const i64 a = as.prod(l, m1 + 1) + x1 + static_cast<i64>(m1 - l + 1 - s1) * (1 - l);
        const i64 b = -as.prod(m1 + 1, r + 1) + y1 + static_cast<i64>(r - m1 - t1) * (r + 1);

        ++l;
        const i32 m2 = (l + r) / 2;
        const auto [s2, x2] = wml2.count(l, m2 + 1, 1 - l);
        const auto [t2, y2] = wmr2.count(m2 + 1, r + 1, r + 1);

        const i64 c = as.prod(l, m2 + 1) + x2 + static_cast<i64>(m2 - l + 1 - s2) * (1 - l);
        const i64 d = -as.prod(m2 + 1, r + 1) + y2 + static_cast<i64>(r - m2 - t2) * (r + 1);

        return a + b + c + d;
    }
};

#endif // LIB_STATIC_PALINDROME_HPP
