#ifndef LIB_FENWICK_TREE_HPP
#define LIB_FENWICK_TREE_HPP 1

#include <cassert>
#include <vector>

#include <lib/algebraic_traits.hpp>
#include <lib/prelude.hpp>

template <typename Monoid, is_abelian_group_t<Monoid> * = nullptr>
struct fenwick_tree {
    using MX = Monoid;
    using X = typename MX::ValueT;

    i32 n;
    X t;
    std::vector<X> d;

    fenwick_tree() {}
    explicit fenwick_tree(i32 m) {
        build(m);
    }

    explicit fenwick_tree(const std::vector<X> &v) {
        build(v);
    }

    template <typename F>
    fenwick_tree(i32 m, F f) {
        build(m, f);
    }

    void build(i32 m) {
        build(m, [](i32) -> X { return MX::unit(); });
    }

    void build(const std::vector<X> &v) {
        build(static_cast<i32>(v.size()), [&](i32 i) -> X { return v[i]; });
    }

    template <typename F>
    void build(i32 m, F f) {
        n = m;
        t = MX::unit();
        d.assign(n, MX::unit());

        for (i32 i = 0; i < n; ++i) d[i] = f(i);
        for (i32 i = 1; i <= n; ++i) {
            const i32 j = i + (i & -i);
            if (j <= n) d[j - 1] = MX::op(d[i - 1], d[j - 1]);
        }

        t = prod(n);
    }

    X prod_all() const {
        return t;
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

    void multiply(i32 p, const X &x) {
        assert(0 <= p && p < n);

        t = MX::op(t, x);
        for (++p; p <= n; p += p & -p) d[p - 1] = MX::op(d[p - 1], x);
    }

    void set(i32 p, const X &x) {
        multiply(p, MX::op(MX::inv(get(p)), x));
    }

    template <typename F>
    i32 max_right(F f, i32 l) const {
        assert(0 <= l && l <= n && f(MX::unit()));

        MX s = MX::unit();
        i32 i = l;

        i32 k;
        for (;;) {
            if (i & 1) s = MX::op(s, MX::inv(d[--i]));
            if (i == 0) {
                k = topbit(n) + 1;
                break;
            }

            k = lowbit(i) - 1;
            if (i + (1 << k) > n) break;
            if (const MX u = MX::op(s, d[i + (1 << k) - 1]); !f(u)) break;

            s = MX::op(s, MX::inv(d[i - 1]));
            i -= i & -i;
        }

        while (k--) {
            if (i + (1 << k) - 1 < n) {
                if (const MX u = MX::op(s, d[i + (1 << k) - 1]); f(u)) {
                    i += 1 << k;
                    s = u;
                }
            }
        }

        return i;
    }

    template <typename F>
    i32 min_left(F f, i32 r) const {
        assert(0 <= r && r <= n && f(MX::unit()));

        MX s = MX::unit();
        i32 i = r;

        i32 k = 0;
        while (i > 0 && f(s)) {
            s = MX::op(s, d[i - 1]);
            k = lowbit(i);
            i -= i & -i;
        }

        if (f(s)) {
            assert(i == 0);
            return 0;
        }

        while (k--) {
            if (const MX u = MX::op(s, MX::inv(d[i + (1 << k) - 1])); !f(u)) {
                i += 1 << k;
                s = u;
            }
        }
    }

    i32 kth(X k, i32 l) const {
        return max_right([&k](X x) -> bool { return x <= k; }, l);
    }
};

#endif // LIB_FENWICK_TREE_HPP
