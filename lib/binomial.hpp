#ifndef LIB_BINOMIAL_HPP
#define LIB_BINOMIAL_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>

template <typename Z>
struct binomial {
    i32 n;
    std::vector<Z> fs, gs;

    binomial() {}

    explicit binomial(i32 m) {
        build(m);
    }

    void build(i32 m) {
        n = m;
        fs.resize(n + 1);
        gs.resize(n + 1);

        fs[0] = 1;
        for (i32 i = 1; i <= n; ++i) fs[i] = fs[i - 1] * i;

        gs[n] = 1 / fs[n];
        for (i32 i = n; i > 0; --i) gs[i - 1] = gs[i] * i;
    }

    Z comb(i32 p, i32 q) const {
        assert(0 <= p && p <= n);
        assert(0 <= q && q <= p);

        return fs[p] * gs[q] * gs[p - q];
    }
};

#endif // LIB_BINOMIAL_HPP
