#ifndef LIB_HLD_LAZY_SEGMENT_TREE_HPP
#define LIB_HLD_LAZY_SEGMENT_TREE_HPP 1

#include <vector>
#include <algorithm>
#include <lib/prelude.hpp>
#include <lib/monoids/reverse_monoid.hpp>
#include <lib/hld.hpp>
#include <lib/lazy_segment_tree.hpp>

template <class ActedMonoid>
struct hld_lazy_segment_tree {
	using AM = ActedMonoid;
	
        using MX = AM::MX;
	using MA = AM::MA;

        using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	using Tree = std::vector<std::vector<i32>>;
	hld h;

	struct RAM {
		using MX = monoid_reverse_monoid<MX>;
		using MA = MA;

		using X = MX::ValueT;
		using A = MA::ValueT;

		static constexpr X act(const X& x, const A& a, const i64 size) {
			return AM::act(x, a, size);
		}
	};

	lazy_segment_tree<ActedMonoid> st;
	lazy_segment_tree<RAM> str;

        explicit hld_lazy_segment_tree(const Tree &g):
		h(g) {
                build();
        }

        template <typename F>
        hld_lazy_segment_tree(const Tree& g, F f):
		h(g) {
                build(f);
        }

        explicit hld_lazy_segment_tree(const Tree& g, const std::vector<X> &v):
		h(g) {
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
		if constexpr (!MX::commutative) str.build(h.n, f);
        }

        X get(i32 u) {
                return st.get(h.tin[u]);
        }

        void set(i32 u, const X &x) {
		st.set(h.tin[u], x);
		if constexpr (!MX::commutative) str.set(h.tin[u], x);
        }

        void multiply(i32 u, const X &x) {
		st.multiply(h.tin[u], x);
		if constexpr (!MX::commutative) str.multiply(h.tin[u], x);
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

	X prod_all() { return st.prod_all(); }

	X prod(i32 u, i32 v) {
		const i32 a = h.tin[u];
		const i32 b = h.tin[v];

		if (a <= b) return st.prod(a, b + 1);
		if constexpr (!MX::commutative) return str.prod(b, a + 1);

		return st.prod(b, a + 1);
	}

	void apply_path(i32 u, i32 v, A a) {
		for (const auto &[s, t] : h.decompose(u, v)) {
			const auto &[x, y] = std::minmax(h.tin[s], h.tin[t]);

			st.apply(x, y + 1, a);
			if constexpr (!MX::commutative) str.apply(x, y + 1, a);
		}
	}

	void apply_subtree(i32 u, A a) {
		const i32 x = h.tin[u];
		const i32 y = h.tin[u] + h.sz[u];

		st.apply(x, y, a);
		if constexpr (!MX::commutative) str.apply(x, y, a);
	}
};

#endif // LIB_HLD_LAZY_SEGMENT_TREE_HPP
