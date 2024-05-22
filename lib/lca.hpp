#ifndef LIB_LCA_HPP
#define LIB_LCA_HPP 1

#include <algorithm>
#include <vector>

#include "lib/sparse_table.hpp"

struct LCA {
public:
	using Graph = std::vector<std::vector<int>>;

	explicit LCA(const Graph &g):
		n{static_cast<int>(g.size())},
		s(n), e(n), d(n),
		rmq{tour(g)}
	{}

	bool anc(int u, int v) const {
		assert(0 <= u && u < n && 0 <= v && v < n);

		return s[u] <= s[v] && e[u] >= e[v];
	}

	int lca(int u, int v) const {
		assert(0 <= u && u < n && 0 <= v && v < n);

		if (u == v) return u;

		const auto &[l, r] = std::minmax(s[u], s[v]);
		return p[rmq.prod(l, r)];
	}

	int dist(int u, int v) const {
		assert(0 <= u && u < n && 0 <= v && v < n);

		return d[u] + d[v] - 2 * d[lca(u, v)];
	}

private:
	std::vector<int> tour(const Graph &g) {
		p.reserve(2 * n - 1);
		h.reserve(2 * n - 1);

		dfs(g, 0, 0);
		return h;
	}

	void dfs(const Graph &g, const int u, const int t) {
		s[u] = static_cast<int>(p.size());

		for (const auto v : g[u]) {
			if (v == t) continue;

			d[v] = d[u] + 1;
			p.push_back(u);
			h.push_back(s[u]);

			dfs(g, v, u);
		}

		e[u] = static_cast<int>(p.size()) - 1;
	}

	int n;
	std::vector<int> p, h, s, e, d;

	SparseTable<int, [](const int a, const int b) { return std::min(a, b); }> rmq;
};

#endif // LIB_LCA_HPP
