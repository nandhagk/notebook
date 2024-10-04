#ifndef LIB_HLD_HPP
#define LIB_HLD_HPP 1

#include <vector>
#include <cassert>
#include <lib/prelude.hpp>

struct hld {
public:
	using Graph = std::vector<std::vector<i32>>;

	explicit hld(const Graph& g): hld(g, 0) {}

	hld(const Graph& g, const i32 root):
		n(static_cast<i32>(g.size())),
		sz(n, 1), tin(n), depth(n), par(n), tour(n), best(n, -1), start(n)
	{
		par[root] = -1;
		dfs_sz(g, root);
		start[root] = root;
		dfs_hld(g, root);
	}

	bool is_ancestor(i32 u, i32 v) const {
		assert(0 <= u && u < n);
		assert(0 <= v && v < n);

		return tin[u] <= tin[v] && tin[u] + sz[u] > tin[v];
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

	i32 n;
	std::vector<i32> sz, tin, depth, par, tour, best, start;

private:
	void dfs_sz(const Graph& g, i32 u) {
		i32 &x = best[u];
		const i32 t = par[u];

		for (const i32 v : g[u]) {
			if (v == t) continue;

			par[v] = u;
			depth[v] = depth[u] + 1;

			dfs_sz(g, v);

			sz[u] += sz[v];
			if (x == -1 || sz[v] > sz[x]) x = v;
		}
	}

	void dfs_hld(const Graph& g, i32 u) {
		static i32 time = 0;

		tour[time] = u;
		tin[u] = time++;

		const i32 x = best[u];
		const i32 t = par[u];

		if (x != -1) {
			start[x] = start[u];
			dfs_hld(g, x);
		}

		for (const i32 v : g[u]) {
			if (v == t || v == x) continue;

			start[v] = v;
			dfs_hld(g, v);
		}
	}
};

#endif // LIB_HLD_HPP
