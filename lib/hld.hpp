#ifndef LIB_HLD_HPP
#define LIB_HLD_HPP 1

#include <vector>
#include <algorithm>
#include <cassert>

#include <lib/prelude.hpp>
#include <lib/monoids/reverse_monoid.hpp>
#include <lib/segment_tree.hpp>
#include <lib/lazy_segment_tree.hpp>

struct hld {
	using Tree = std::vector<std::vector<i32>>;

	i32 n, time;
	const Tree& g;
	std::vector<i32> sz, tin, depth, par, tour, best, start;

	explicit hld(const Tree& t): hld(t, 0) {}

	hld(const Tree& t, const i32 root):
		n(static_cast<i32>(t.size())), time{}, g(t),
		sz(n, 1), tin(n), depth(n), par(n), tour(n), best(n, -1), start(n)
	{
		par[root] = -1;
		dfs_sz(root);
		start[root] = root;
		dfs_hld(root);
	}

	bool is_ancestor(i32 u, i32 v) const {
		assert(0 <= u && u < n);
		assert(0 <= v && v < n);

		return tin[u] <= tin[v] && tin[u] + sz[u] > tin[v];
	}

	i32 jump(i32 u, i32 k) const {
		assert(0 <= u && u < n);
		assert(0 <= k);

		if (depth[u] < k) return -1;

		while (u != -1) {
			const i32 s = start[u];
			if (depth[s] <= depth[u] - k) {
				return tour[tin[u] - k];
			}

			k -= depth[u] - depth[s] + 1;
			u = par[s];
		}

		return u;
	}

	i32 lca(i32 u, i32 v) const {
		assert(0 <= u && u < n);
		assert(0 <= v && v < n);

		if (is_ancestor(u, v)) return u;
		if (is_ancestor(v, u)) return v;

		for (; start[u] != start[v]; v = par[start[v]]) {
			if (depth[start[u]] > depth[start[v]]) std::swap(u, v);
		}

		return depth[u] < depth[v] ? u : v;
	}

	i32 dist(i32 u, i32 v) const {
		assert(0 <= u && u < n);
		assert(0 <= v && v < n);

		return depth[u] + depth[v] - 2 * depth[lca(u, v)];
	}

	bool is_on_path(i32 u, i32 v, i32 s) const {
		return lca(u, v) == s || (is_ancestor(s, u) ^ is_ancestor(s, v));
	}

	i32 jump(i32 u, i32 v, i32 k) const {
		assert(0 <= u && u < n);
		assert(0 <= v && v < n);
		assert(0 <= k);

		const i32 du = depth[u];
		const i32 dv = depth[v];
		const i32 dx = depth[lca(u, v)];

		const i32 l = du - dx;
		const i32 r = dv - dx;

		if (l + r < k) {
			return -1;
		} else if (k < l) {
			return jump(u, k);
		} else {
			return jump(v, l + r - k);
		}
	}

	std::vector<std::pair<i32, i32>> decompose(i32 u, i32 v) const {
		assert(0 <= u && u < n);
		assert(0 <= v && v < n);

		std::vector<std::pair<i32, i32>> up, dn;

		while (start[u] != start[v]) {
			if (tin[u] < tin[v]) {
				dn.emplace_back(start[v], v);
				v = par[start[v]];
			} else {
				up.emplace_back(u, start[u]);
				u = par[start[u]];
			}
		}

		if (tin[u] < tin[v]) {
			dn.emplace_back(u, v);
		} else {
			up.emplace_back(u, v);
		}

		up.insert(up.end(), dn.rbegin(), dn.rend());
		return up;
	}

private:
	void dfs_sz(i32 u) {
		i32 &x = best[u];
		const i32 t = par[u];

		for (const i32 v : g[u]) {
			if (v == t) continue;

			par[v] = u;
			depth[v] = depth[u] + 1;

			dfs_sz(v);

			sz[u] += sz[v];
			if (x == -1 || sz[v] > sz[x]) x = v;
		}
	}

	void dfs_hld(i32 u) {
		tour[time] = u;
		tin[u] = time++;

		const i32 x = best[u];
		const i32 t = par[u];

		if (x != -1) {
			start[x] = start[u];
			dfs_hld(x);
		}

		for (const i32 v : g[u]) {
			if (v == t || v == x) continue;

			start[v] = v;
			dfs_hld(v);
		}
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

	X prod_all() { return st.prod_all(); }

	X prod(i32 u, i32 v) {
		const i32 a = h.tin[u];
		const i32 b = h.tin[v];

		if (a <= b) return st.prod(a, b + 1);
		if constexpr (!MX::commutative) return rst.prod(b, a + 1);

		return st.prod(b, a + 1);
	}
};

template <class ActedMonoid>
struct hld_lazy_segment_tree {
	using AM = ActedMonoid;
	
        using MX = typename AM::MX;
	using MA = typename AM::MA;

        using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	const hld& h;

	struct RAM {
		using MX = monoid_reverse_monoid<hld_lazy_segment_tree::MX>;
		using MA = hld_lazy_segment_tree::MA;

		using X = typename MX::ValueT;
		using A = typename MA::ValueT;

		static constexpr X act(const X& x, const A& a, const i64 size) {
			return AM::act(x, a, size);
		}
	};

	lazy_segment_tree<ActedMonoid> st;
	lazy_segment_tree<RAM> rst;

        explicit hld_lazy_segment_tree(const hld &g): h(g) {
                build();
        }

        template <typename F>
        hld_lazy_segment_tree(const hld& g, F f): h(g) {
                build(f);
        }

        explicit hld_lazy_segment_tree(const hld& g, const std::vector<X> &v): h(g) {
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

	X prod_all() { return st.prod_all(); }

	X prod(i32 u, i32 v) {
		const i32 a = h.tin[u];
		const i32 b = h.tin[v];

		if (a <= b) return st.prod(a, b + 1);
		if constexpr (!MX::commutative) return rst.prod(b, a + 1);

		return st.prod(b, a + 1);
	}

	void apply_path(i32 u, i32 v, A a) {
		for (const auto &[s, t] : h.decompose(u, v)) {
			const auto &[x, y] = std::minmax(h.tin[s], h.tin[t]);

			st.apply(x, y + 1, a);
			if constexpr (!MX::commutative) rst.apply(x, y + 1, a);
		}
	}

	void apply_subtree(i32 u, A a) {
		const i32 x = h.tin[u];
		const i32 y = h.tin[u] + h.sz[u];

		st.apply(x, y, a);
		if constexpr (!MX::commutative) rst.apply(x, y, a);
	}
};

#endif // LIB_HLD_HPP
