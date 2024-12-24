#ifndef LIB_FENWICK_TREE_HPP
#define LIB_FENWICK_TREE_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_abelian_group_t<Monoid> * = nullptr> struct fenwick_tree {
    using MX = Monoid;
    using X = typename MX::ValueT;

    i32 n;
    std::vector<X> d;

    fenwick_tree() {}
    explicit fenwick_tree(i32 m) { build(m); }

    explicit fenwick_tree(const std::vector<X> &v) { build(v); }

    template <typename F> fenwick_tree(i32 m, F f) { build(m, f); }

    void build(i32 m) {
        build(m, [](i32) -> X { return MX::unit(); });
    }

    void build(const std::vector<X> &v) {
        build(static_cast<i32>(v.size()), [&](i32 i) -> X { return v[i]; });
    }

    template <typename F> void build(i32 m, F f) {
        n = m;
        d.assign(n, MX::unit());

        for (i32 i = 0; i < n; ++i) d[i] = f(i);
        for (i32 i = 1; i <= n; ++i) {
            const i32 j = i + (i & -i);
            if (j <= n) d[j - 1] = MX::op(d[i - 1], d[j - 1]);
        }
    }

    X prod(i32 r) const {
        assert(0 <= r && r <= n);

        X x = MX::unit();
        for (; r > 0; r -= r & -r) x = MX::op(x, d[r - 1]);
        return x;
    }

    X prod(i32 l, i32 r) const {
        assert(0 <= l && l <= r && r <= n);

        if (l == 0) return prod(r);

        X vl = MX::unit();
        X vr = MX::unit();

        for (; l < r; r -= r & -r) vr = MX::op(vr, d[r - 1]);
        for (; r < l; l -= l & -l) vl = MX::op(vl, d[l - 1]);

        return MX::op(MX::inv(vl), vr);
    }

    X get(i32 p) const {
        assert(0 <= p && p < n);

        return prod(p, p + 1);
    }

    void multiply(i32 p, X x) {
        assert(0 <= p && p < n);

        for (++p; p <= n; p += p & -p) d[p - 1] = MX::op(d[p - 1], x);
    }

    void set(i32 p, X x) { multiply(p, MX::op(MX::inv(get(p)), x)); }
};

#endif // LIB_FENWICK_TREE_HPP
