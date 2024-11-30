#ifndef LIB_HLD_SEGMENT_TREE_HPP
#define LIB_HLD_SEGMENT_TREE_HPP 1

#include <lib/prelude.hpp>
#include <lib/hld.hpp>
#include <lib/segment_tree.hpp>
#include <lib/monoids/reverse_monoid.hpp>

template <typename Monoid>
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
		static_assert(MX::commutative);
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

#endif // LIB_HLD_SEGMENT_TREE_HPP
