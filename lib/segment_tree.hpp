#ifndef LIB_SEGMENT_TREE_HPP
#define LIB_SEGMENT_TREE_HPP 1

#include <cassert>
#include <vector>
#include <lib/prelude.hpp>

template <class Monoid>
struct segment_tree {
        using MX = Monoid;
        using X = typename MX::ValueT;

        i32 n, log, size;
        std::vector<X> d;

        segment_tree() {}
        explicit segment_tree(i32 m) {
                build(m);
        }

        template <typename F>
        segment_tree(i32 m, F f) {
                build(m, f);
        }

        explicit segment_tree(const std::vector<X> &v) {
                build(v);
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
		size = std::bit_ceil(static_cast<u32>(n));
		log = lowbit(size);

                d.assign(size << 1, MX::unit());

                for (i32 i = 0; i < n; ++i) d[size + i] = f(i);
                for (i32 i = size - 1; i >= 1; --i) update(i);
        }

        X get(i32 i) {
                return d[size + i];
        }

        std::vector<X> get_all() {
                return {d.begin() + size, d.begin() + size + n};
        }

        void update(i32 i) {
                d[i] = Monoid::op(d[2 * i], d[2 * i + 1]);
        }

        void set(i32 i, const X &x) {
                assert(i < n);

                i += size;
                d[i] = x;

                while (i >>= 1) update(i);
        }

        void multiply(i32 i, const X &x) {
                assert(i < n);

                i += size;
                d[i] = Monoid::op(d[i], x);

                while (i >>= 1) update(i);
        }

        X prod(i32 l, i32 r) {
                assert(0 <= l && l <= r && r <= n);

                X vl = Monoid::unit(), vr = Monoid::unit();
                l += size, r += size;

                while (l < r) {
                        if (l & 1) vl = Monoid::op(vl, d[l++]);
                        if (r & 1) vr = Monoid::op(d[--r], vr);

                        l >>= 1;
                        r >>= 1;
                }

                return Monoid::op(vl, vr);
        }

        X prod_all() { 
                return d[1]; 
        }

        template <class F>
        i32 max_right(F f, i32 l) {
                assert(0 <= l && l <= n && f(Monoid::unit()));

                if (l == n) return n;

                l += size;
                X sm = Monoid::unit();
                do {
                        while (l % 2 == 0) l >>= 1;

                        if (!f(Monoid::op(sm, d[l]))) {
                                while (l < size) {
                                        l = 2 * l;
                                        if (f(Monoid::op(sm, d[l]))) {
                                                sm = Monoid::op(sm, d[l++]);
                                        }
                                }

                                return l - size;
                        }

                        sm = Monoid::op(sm, d[l++]);
                } while ((l & -l) != l);

                return n;
        }

        template <class F>
        i32 min_left(F f, i32 r) {
                assert(0 <= r && r <= n && f(Monoid::unit()));

                if (r == 0) return 0;

                r += size;
                X sm = Monoid::unit();
                do {
                        --r;
                        while (r > 1 && (r % 2)) r >>= 1;

                        if (!f(Monoid::op(d[r], sm))) {
                                while (r < size) {
                                        r = 2 * r + 1;
                                        if (f(Monoid::op(d[r], sm))) {
                                                sm = Monoid::op(d[r--], sm);
                                        }
                                }

                                return r + 1 - size;
                        }

                        sm = Monoid::op(d[r], sm);
                } while ((r & -r) != r);

                return 0;
        }
};

#endif // LIB_SEGMENT_TREE_HPP
