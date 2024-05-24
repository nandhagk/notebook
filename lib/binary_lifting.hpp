#ifndef LIB_BINARY_LIFTING_HPP
#define LIB_BINARY_LIFTING_HPP

#include <vector>
#include <cassert>

class BinaryLifting {
public:
	using Graph = std::vector<std::vector<int>>;

	explicit BinaryLifting(const Graph &g): BinaryLifting(g, 0) {}

	BinaryLifting(const Graph &g, const int root):
		n(static_cast<int>(g.size())),
		log(std::bit_width(static_cast<unsigned int>(n))),
		tin(n), tout(n), depth(n),
		par(n, std::vector(log, root))
	{
		dfs(g, root);

		for (auto j = 1; j < log; ++j) {
			for (auto i = 0; i < n; ++i) {
				par[i][j] = par[par[i][j - 1]][j - 1];
			}
		}
	}

	bool is_ancestor(int u, int v) const {
		assert(0 <= u && u < n && 0 <= v && v < n);

		return tin[u] <= tin[v] && tout[u] >= tout[v];
	}

	int jump(int u, int k) const {
		assert(0 <= u && u < n && 0 <= k);

		if (depth[u] < k) return -1;

		for (auto i = log - 1; i >= 0; --i) {
			if ((k >> i) & 1) u = par[u][i];
		}

		return u;
	}

	int lca(int u, int v) const {
		assert(0 <= u && u < n && 0 <= v && v < n);

		if (is_ancestor(u, v)) return u;
		if (is_ancestor(v, u)) return v;

		for (auto i = log - 1; i >= 0; --i) {
			if (!is_ancestor(par[v][i], u)) v = par[v][i];
		}

		return par[v][0];
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
	const int n, log;
	std::vector<int> tin, tout, depth;
	std::vector<std::vector<int>> par;

	void dfs(const Graph& g, int u) {
		static int time = 0;

		tin[u] = time++;
		for (const auto v : g[u]) {
			if (v == par[u][0]) continue;

			par[v][0] = u;
			depth[v] = depth[u] + 1;

			dfs(g, v);
		}

		tout[u] = time;
	}
};


#endif // LIB_BINARY_LIFTING_HPP
