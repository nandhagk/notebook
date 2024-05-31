#ifndef LIB_BINARY_LIFTING_HPP
#define LIB_BINARY_LIFTING_HPP 1

#include <vector>
#include <cassert>
#include <lib/prelude.hpp>

class BinaryLifting {
public:
	using Graph = std::vector<std::vector<i32>>;

	explicit BinaryLifting(const Graph &g): BinaryLifting(g, 0) {}

	BinaryLifting(const Graph &g, const i32 root):
		n(static_cast<i32>(g.size())),
		log(std::bit_width(static_cast<u32>(n))),
		tin(n), tout(n), depth(n),
		par(n, std::vector(log, root))
	{
		dfs(g, root);

		for (i32 j = 1; j < log; ++j) {
			for (i32 i = 0; i < n; ++i) {
				par[i][j] = par[par[i][j - 1]][j - 1];
			}
		}
	}

	bool is_ancestor(i32 u, i32 v) const {
		assert(0 <= u && u < n && 0 <= v && v < n);

		return tin[u] <= tin[v] && tout[u] >= tout[v];
	}

	i32 jump(i32 u, i32 k) const {
		assert(0 <= u && u < n && 0 <= k);

		if (depth[u] < k) return -1;

		for (i32 i = log - 1; i >= 0; --i) {
			if ((k >> i) & 1) u = par[u][i];
		}

		return u;
	}

	i32 lca(i32 u, i32 v) const {
		assert(0 <= u && u < n && 0 <= v && v < n);

		if (is_ancestor(u, v)) return u;
		if (is_ancestor(v, u)) return v;

		for (i32 i = log - 1; i >= 0; --i) {
			if (!is_ancestor(par[v][i], u)) v = par[v][i];
		}

		return par[v][0];
	}

	i32 dist(i32 u, i32 v) const {
		assert(0 <= u && u < n && 0 <= v && v < n);

		return depth[u] + depth[v] - 2 * depth[lca(u, v)];
	}

	i32 jump(i32 u, i32 v, i32 k) const {
		assert(0 <= u && u < n && 0 <= v && v < n && 0 <= k);

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

private:
	const i32 n, log;
	std::vector<i32> tin, tout, depth;
	std::vector<std::vector<i32>> par;

	void dfs(const Graph& g, i32 u) {
		static i32 time = 0;

		tin[u] = time++;
		for (const i32 v : g[u]) {
			if (v == par[u][0]) continue;

			par[v][0] = u;
			depth[v] = depth[u] + 1;

			dfs(g, v);
		}

		tout[u] = time;
	}
};


#endif // LIB_BINARY_LIFTING_HPP
