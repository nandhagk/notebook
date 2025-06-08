#ifndef LIB_DISJOINT_SPARSE_TABLE_HPP
#define LIB_DISJOINT_SPARSE_TABLE_HPP 1

#include <cassert>
#include <vector>

#include <lib/algebraic_traits.hpp>
#include <lib/bits.hpp>
#include <lib/prelude.hpp>

template <typename Monoid, is_monoid_t<Monoid> * = nullptr>
struct disjoint_sparse_table {
    using MX = Monoid;
    using X = typename MX::ValueT;

    i32 n, log;
    std::vector<std::vector<X>> d;

    disjoint_sparse_table() {}
    explicit disjoint_sparse_table(i32 m) {
        build(m);
    }

    explicit disjoint_sparse_table(const std::vector<X> &v) {
        build(v);
    }

    template <typename F>
    disjoint_sparse_table(i32 m, F f) {
        build(m, f);
    }

    void build(const std::vector<X> &v) {
        build(static_cast<i32>(v.size()), [&](i32 i) -> X { return v[i]; });
    }

    template <typename F>
    void build(i32 m, F f) {
        n = m;

        log = 1;
        while ((1 << log) < n) ++log;
        d.resize(log);

        d[0].resize(n);
        for (i32 i = 0; i < n; ++i) d[0][i] = f(i);

        for (i32 i = 1; i < log; ++i) {
            auto &v = d[i];
            v = d[0];

            const i32 k = 1 << i;
            for (i32 j = k; j <= n; j += 2 * k) {
                const i32 l = j - k;
                const i32 r = std::min(n, j + k);

                for (i32 p = j - 1; p >= l + 1; --p) v[p - 1] = MX::op(v[p - 1], v[p]);
                for (i32 p = j; p < r - 1; ++p) v[p + 1] = MX::op(v[p], v[p + 1]);
            }
        }
    }

    X get(i32 p) const {
        assert(0 <= p && p < n);

        return prod(p, p + 1);
    }

    X prod(i32 l, i32 r) const {
        assert(0 <= l && l <= r && r <= n);

        --r;
        if (l == r + 1) return MX::unit();
        if (l == r) return d[0][l];

        const i32 k = topbit(l ^ r);
        return MX::op(d[k][l], d[k][r]);
    }
};

#endif // LIB_DISJOINT_SPARSE_TABLE_HPP
