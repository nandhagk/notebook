#ifndef LIB_SEGMENT_TREE_HPP
#define LIB_SEGMENT_TREE_HPP 1

#include <cassert>
#include <vector>
#include <lib/prelude.hpp>

template <class Monoid>
struct SegmentTree {
        using MX = Monoid;
        using X = typename MX::ValueT;

        std::vector<X> d;
        i32 n, log, size;

        SegmentTree() {}
        explicit SegmentTree(i32 m) {
                Build(m);
        }

        template <typename F>
        SegmentTree(i32 m, F f) {
                Build(m, f);
        }

        explicit SegmentTree(const std::vector<X> &v) {
                Build(v);
        }

        void Build(i32 m) {
                Build(m, [](i32) -> X { return MX::Unit(); });
        }

        void Build(const std::vector<X> &v) {
                Build(static_cast<i32>(v.size()), [&](i32 i) -> X { return v[i]; });
        }

        template <typename F>
        void Build(i32 m, F f) {
                n = m;
		size = std::bit_ceil(static_cast<u32>(n));
		log = lowbit(size);

                d.assign(size << 1, MX::Unit());

                for (i32 i = 0; i < n; ++i) d[size + i] = f(i);
                for (i32 i = size - 1; i >= 1; --i) Update(i);
        }

        X Get(i32 i) {
                return d[size + i];
        }

        std::vector<X> GetAll() {
                return {d.begin() + size, d.begin() + size + n};
        }

        void Update(i32 i) {
                d[i] = Monoid::Op(d[2 * i], d[2 * i + 1]);
        }

        void Set(i32 i, const X &x) {
                assert(i < n);

                i += size;
                d[i] = x;

                while (i >>= 1) Update(i);
        }

        void Multiply(i32 i, const X &x) {
                assert(i < n);

                i += size;
                d[i] = Monoid::Op(d[i], x);

                while (i >>= 1) Update(i);
        }

        X Prod(i32 l, i32 r) {
                assert(0 <= l && l <= r && r <= n);

                X vl = Monoid::Unit(), vr = Monoid::Unit();
                l += size, r += size;

                while (l < r) {
                        if (l & 1) vl = Monoid::Op(vl, d[l++]);
                        if (r & 1) vr = Monoid::Op(d[--r], vr);

                        l >>= 1;
                        r >>= 1;
                }

                return Monoid::Op(vl, vr);
        }

        X ProdAll() { 
                return d[1]; 
        }

        template <class F>
        i32 MaxRight(F f, i32 l) {
                assert(0 <= l && l <= n);
                assert(f(Monoid::Unit()));

                if (l == n) return n;

                l += size;
                X sm = Monoid::Unit();
                do {
                        while (l % 2 == 0) l >>= 1;

                        if (!f(Monoid::Op(sm, d[l]))) {
                                while (l < size) {
                                        l = 2 * l;
                                        if (f(Monoid::Op(sm, d[l]))) {
                                                sm = Monoid::Op(sm, d[l++]);
                                        }
                                }

                                return l - size;
                        }

                        sm = Monoid::Op(sm, d[l++]);
                } while ((l & -l) != l);

                return n;
        }

        template <class F>
        i32 MinLeft(F f, i32 r) {
                assert(0 <= r && r <= n)
                assert(f(Monoid::Unit()));

                if (r == 0) return 0;

                r += size;
                X sm = Monoid::Unit();
                do {
                        --r;
                        while (r > 1 && (r % 2)) r >>= 1;

                        if (!f(Monoid::Op(d[r], sm))) {
                                while (r < size) {
                                        r = 2 * r + 1;
                                        if (f(Monoid::Op(d[r], sm))) {
                                                sm = Monoid::Op(d[r--], sm);
                                        }
                                }

                                return r + 1 - size;
                        }

                        sm = Monoid::Op(d[r], sm);
                } while ((r & -r) != r);

                return 0;
        }
};

#endif // LIB_SEGMENT_TREE_HPP
