#ifndef LIB_BCC_HPP
#define LIB_BCC_HPP 1

#include <vector>
#include <cassert>
#include <lib/prelude.hpp>

struct bcc {
	using Graph = std::vector<std::vector<i32>>;

	i32 n, time, group;
	const Graph& g;
	std::vector<i32> tin, low, ids;

	explicit bcc(const Graph &t):
		n(static_cast<i32>(t.size())), time{}, group{}, g(t), 
		tin(n, -1), low(n), ids(n, -1) {
		build();
	}

	bool is_bridge(i32 u, i32 v) const {
		assert(0 <= u && u < n);
		assert(0 <= v && v < n);

		if (tin[u] > tin[v]) std::swap(u, v);
		return low[v] > tin[u];
	}

private:
	void build() {
		for (i32 u = 0; u < n; ++u) {
			if (tin[u] != -1) continue;
			dfs(u);
		}

		for (i32 u = 0; u < n; ++u) {
			if (ids[u] != -1) continue;
			dfs_id(u);

			++group;
		}
	}

	void dfs(i32 u, i32 t = -1) {
		tin[u] = low[u] = time++;
	 
		i32 cnt{};
		for (const i32 v : g[u]) {
			if (v == t && !cnt) {
				++cnt;
				continue;
			}
 
			if (tin[v] == -1) {
				dfs(v, u);
				low[u] = std::min(low[u], low[v]);

			} else {
				low[u] = std::min(low[u], tin[v]);
			}
		}
	}

	void dfs_id(i32 u) {
		ids[u] = group;

		for (const i32 v : g[u]) {
			if (ids[v] != -1 || is_bridge(u, v)) continue;
			dfs_id(v);
		}
	}

};

#endif // LIB_BCC_HPP
