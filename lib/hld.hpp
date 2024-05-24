#ifndef LIB_HLD_CPP
#define LIB_HLD_CPP

#include <vector>
#include <cassert>

struct HLD {
public:
	using Graph = std::vector<std::vector<int>>;

	explicit HLD(const Graph& g): HLD(g, 0) {}

	HLD(const Graph& g, const int root):
		n(static_cast<int>(g.size())),
		sz(n, 1), tin(n), depth(n), par(n), tour(n), best(n, -1), start(n)
	{
		par[root] = -1;
		dfs_sz(g, root);
		start[root] = root;
		dfs_hld(g, root);
	}

	bool is_ancestor(int u, int v) const {
		assert(0 <= u && u < n && 0 <= v && v < n);

		return tin[u] <= tin[v] && tin[u] + sz[u] > tin[v];
	}

	int jump(int u, int k) const {
		assert(0 <= u && u < n && 0 <= k);

		if (depth[u] < k) return -1;

		while (u != -1) {
			const auto s = start[u];
			if (depth[s] <= depth[u] - k) {
				return tour[tin[u] - k];
			}

			k -= depth[u] - depth[s] + 1;
			u = par[s];
		}

		return u;
	}

	int lca(int u, int v) const {
		assert(0 <= u && u < n && 0 <= v && v < n);

		if (is_ancestor(u, v)) return u;
		if (is_ancestor(v, u)) return v;

		for (; start[u] != start[v]; v = par[start[v]]) {
			if (depth[start[u]] > depth[start[v]]) std::swap(u, v);
		}

		return depth[u] < depth[v] ? u : v;
	}

	int dist(int u, int v) const {
		assert(0 <= u && u < n && 0 <= v && v < n);

		return depth[u] + depth[v] - 2 * depth[lca(u, v)];
	}

	int jump(int u, int v, int k) const {
		const auto du = depth[u];
		const auto dv = depth[v];
		const auto dx = depth[lca(u, v)];

		const auto l = du - dx;
		const auto r = dv - dx;

		if (l + r < k) {
			return -1;
		} else if (k < l) {
			return jump(u, k);
		} else {
			return jump(v, l + r - k);
		}
	}

private:
	int n;
	std::vector<int> sz, tin, depth, par, tour, best, start;

	void dfs_sz(const Graph& g, int u) {
		auto &x = best[u];
		const auto t = par[u];

		for (const auto v : g[u]) {
			if (v == t) continue;

			par[v] = u;
			depth[v] = depth[u] + 1;

			dfs_sz(g, v);

			sz[u] += sz[v];
			if (x == -1 || sz[v] > sz[x]) x = v;
		}
	}

	void dfs_hld(const Graph& g, int u) {
		static int time = 0;

		tour[time] = u;
		tin[u] = time++;

		const auto x = best[u];
		const auto t = par[u];

		if (x != -1) {
			start[x] = start[u];
			dfs_hld(g, x);
		}

		for (const auto v : g[u]) {
			if (v == t || v == x) continue;

			start[v] = v;
			dfs_hld(g, v);
		}
	}
};

#endif // LIB_HLD_CPP
