#ifndef LIB_SEGMENT_TREE_HPP
#define LIB_SEGMENT_TREE_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_monoid_t<Monoid> * = nullptr>
struct segment_tree {
    using MX = Monoid;
    using X = typename MX::ValueT;

    i32 n, log, size;
    std::vector<X> d;

    segment_tree() {}
    explicit segment_tree(i32 m) {
        build(m);
    }

    explicit segment_tree(const std::vector<X> &v) {
        build(v);
    }

    template <typename F>
    segment_tree(i32 m, F f) {
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

        log = 1;
        while ((1 << log) < n) ++log;

        size = 1 << log;
        d.assign(size << 1, MX::unit());

        for (i32 i = 0; i < n; ++i) d[size + i] = f(i);
        for (i32 i = size - 1; i >= 1; --i) update(i);
    }

    X get(i32 i) const {
        assert(0 <= i && i < n);

        return d[size + i];
    }

    std::vector<X> get_all() const {
        return {d.begin() + size, d.begin() + size + n};
    }

    void set(i32 i, const X &x) {
        assert(0 <= i && i < n);

        i += size;
        d[i] = x;

        while (i >>= 1) update(i);
    }

    void multiply(i32 i, const X &x) {
        assert(0 <= i && i < n);

        i += size;
        d[i] = MX::op(d[i], x);

        while (i >>= 1) update(i);
    }

    X prod(i32 l, i32 r) const {
        assert(0 <= l && l <= r && r <= n);

        X vl = MX::unit();
        X vr = MX::unit();

        for (l += size, r += size; l < r; l >>= 1, r >>= 1) {
            if (l & 1) vl = MX::op(vl, d[l++]);
            if (r & 1) vr = MX::op(d[--r], vr);
        }

        return MX::op(vl, vr);
    }

    X prod_all() const {
        return d[1];
    }

    template <typename F>
    i32 max_right(F f, i32 l) const {
        assert(0 <= l && l <= n && f(MX::unit()));

        if (l == n) return n;

        l += size;
        X sm = MX::unit();

        do {
            while (l % 2 == 0) l >>= 1;

            if (!f(MX::op(sm, d[l]))) {
                while (l < size) {
                    l = 2 * l;
                    if (f(MX::op(sm, d[l]))) sm = MX::op(sm, d[l++]);
                }

                return l - size;
            }

            sm = MX::op(sm, d[l++]);
        } while ((l & -l) != l);

        return n;
    }

    template <typename F>
    i32 min_left(F f, i32 r) const {
        assert(0 <= r && r <= n && f(MX::unit()));

        if (r == 0) return 0;

        r += size;
        X sm = MX::unit();

        do {
            --r;
            while (r > 1 && (r % 2)) r >>= 1;

            if (!f(MX::op(d[r], sm))) {
                while (r < size) {
                    r = 2 * r + 1;
                    if (f(MX::op(d[r], sm))) sm = MX::op(d[r--], sm);
                }

                return r + 1 - size;
            }

            sm = MX::op(d[r], sm);
        } while ((r & -r) != r);

        return 0;
    }

private:
    void update(i32 i) {
        d[i] = MX::op(d[2 * i], d[2 * i + 1]);
    }
};

#endif // LIB_SEGMENT_TREE_HPP
