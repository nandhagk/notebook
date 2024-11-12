#ifndef LIB_SEGMENT_TREE_HPP
#define LIB_SEGMENT_TREE_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/hld.hpp>
#include <lib/monoids/reverse_monoid.hpp>

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

                X vl = Monoid::unit();
                X vr = Monoid::unit();

                l += size;
                r += size;

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

template <class Monoid>
struct hld_segment_tree {
        using MX = Monoid;
        using X = typename MX::ValueT;

        const hld& h;

        segment_tree<MX> st;
        segment_tree<monoid_reverse_monoid<MX>> rst;

        explicit hld_segment_tree(const hld& g): h(g) {
                build();
        }

        template <typename F>
        hld_segment_tree(const hld& g, F f): h(g) {
                build(f);
        }

        explicit hld_segment_tree(const hld& g, const std::vector<X> &v): h(g) {
                build(v);
        }

        void build() {
                build([](i32) -> X { return MX::unit(); });
        }

        void build(const std::vector<X> &v) {
                build([&](i32 u) -> X { return v[h.tour[u]]; });
        }

        template <typename F>
        void build(F f) {
                st.build(h.n, f);
                if constexpr (!MX::commutative) rst.build(h.n, f);
        }

        X get(i32 u) {
                return st.get(h.tin[u]);
        }

        void set(i32 u, const X &x) {
                st.set(h.tin[u], x);
                if constexpr (!MX::commutative) rst.set(h.tin[u], x);
        }

        void multiply(i32 u, const X &x) {
                st.multiply(h.tin[u], x);
                if constexpr (!MX::commutative) rst.multiply(h.tin[u], x);
        }

        X prod_path(i32 u, i32 v) {
                X x = MX::unit();
                for (const auto &[s, t] : h.decompose(u, v)) {
                        x = MX::op(x, prod(s, t));
                }

                return x;
        }

        X prod_subtree(i32 u) {
                return st.prod(h.tin[u], h.tin[u] + h.sz[u]);
        }

        X prod_all() { 
                return st.prod_all(); 
        }

        X prod(i32 u, i32 v) {
                const i32 a = h.tin[u];
                const i32 b = h.tin[v];

                if (a <= b) return st.prod(a, b + 1);
                if constexpr (!MX::commutative) return rst.prod(b, a + 1);

                return st.prod(b, a + 1);
        }
};

#endif // LIB_SEGMENT_TREE_HPP
