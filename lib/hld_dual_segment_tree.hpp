#ifndef LIB_DUAL_SEGMENT_TREE_HPP
#define LIB_DUAL_SEGMENT_TREE_HPP 1

#include <lib/prelude.hpp>
#include <lib/hld.hpp>
#include <lib/dual_segment_tree.hpp>

template <typename Monoid>
struct hld_dual_segment_tree {
	using MA = Monoid;
	using A = typename MA::ValueT;

	const hld& h;

	dual_segment_tree<Monoid> st;

	explicit hld_dual_segment_tree(const hld &g): h(g) {
		build();
	}

	template <typename F>
	hld_dual_segment_tree(const hld& g, F f): h(g) {
		build(f);
	}

	explicit hld_dual_segment_tree(const hld& g, const std::vector<A> &v): h(g) {
		build(v);
	}

	void build() {
		build([](i32) -> A { return MA::unit(); });
	}

	void build(const std::vector<A> &v) {
		build([&](i32 u) -> A { return v[h.tour[u]]; });
	}

	template <typename F>
	void build(F f) {
		st.build(h.n, f);
	}

	A get(i32 u) {
		return st.get(h.tin[u]);
	}

	void set(i32 u, const A &x) {
		st.set(h.tin[u], x);
	}

	void apply_path(i32 u, i32 v, A a) {
		for (const auto &[s, t] : h.decompose(u, v)) {
			const auto &[x, y] = std::minmax(h.tin[s], h.tin[t]);
			st.apply(x, y + 1, a);
		}
	}

	void apply_subtree(i32 u, A a) {
		const i32 x = h.tin[u];
		const i32 y = h.tin[u] + h.sz[u];

		st.apply(x, y, a);
	}
};

#endif // LIB_DUAL_SEGMENT_TREE_HPP
