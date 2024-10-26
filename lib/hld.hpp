#ifndef LIB_HLD_HPP
#define LIB_HLD_HPP 1

#include <vector>
#include <cassert>
#include <lib/prelude.hpp>

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

#endif // LIB_HLD_HPP
