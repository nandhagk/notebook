#ifndef LIB_DUAL_SEGTREE_HPP
#define LIB_DUAL_SEGTREE_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/monoids/reverse_monoid.hpp>
#include <lib/hld.hpp>

template <typename Monoid>
struct dual_segment_tree {
	using MA = Monoid;
	using A = typename MA::ValueT;

	i32 n, log, size;
	std::vector<A> z;

	dual_segment_tree() {}
	explicit dual_segment_tree(i32 m) { 
		build(m); 
	}

	explicit dual_segment_tree(const std::vector<A>& v) {
		build(v); 
	}

	template <typename F>
	dual_segment_tree(i32 m, F f) {
		build(m, f);
	}

	void build(i32 m) {
		build(m, [](i32) -> A { return MA::unit(); });
	}

	void build(const std::vector<A>& v) {
		build(static_cast<i32>(v.size()), [&](i32 i) -> A { return v[i]; });
	}

	template <typename F>
	void build(i32 m, F f) {
		n = m;

		log = 1;
		while ((1 << log) < n) ++log;
		size = 1 << log;

		z.assign(size << 1, MA::unit());
		for (i32 i = 0; i < n; ++i) z[i + size] = f(i);
	}

	void set(i32 p, A x) {
		assert(0 <= p && p < n);

		p += size;
		for (i32 i = log; i >= 1; i--) push(p >> i);

		z[p] = x;
	}

	A get(i32 p) {
		assert(0 <= p && p < n);

		p += size;
		for (i32 i = log; i >= 1; i--) push(p >> i);

		return z[p];
	}

	std::vector<A> get_all() {
		for (i32 i = 0; i < size; ++i) push(i);
		return {z.begin() + size, z.begin() + size + n};
	}

	void apply(i32 l, i32 r, A a) {
		assert(0 <= l && l <= r && r <= n);

		if (l == r) return;

		l += size;
		r += size;

		if (!MA::commutative) {
			for (i32 i = log; i >= 1; i--) {
				if (((l >> i) << i) != l) push(l >> i);
				if (((r >> i) << i) != r) push((r - 1) >> i);
			}
		}

		while (l < r) {
			if (l & 1) all_apply(l++, a);
			if (r & 1) all_apply(--r, a);

			l >>= 1;
			r >>= 1;
		}
	}

private:
	void all_apply(i32 k, A a) {
		z[k] = MA::op(z[k], a);
	}

	void push(i32 k) {
		if (z[k] == MA::unit()) return;

		all_apply(2 * k, z[k]);
		all_apply(2 * k + 1, z[k]);

		z[k] = MA::unit();
	}
};

template <class Monoid>
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

#endif // LIB_DUAL_SEGTREE_HPP
