#ifndef LIB_BCC_HPP
#define LIB_BCC_HPP 1

#include <vector>
#include <lib/prelude.hpp>

struct bcc {
	using Graph = std::vector<std::vector<i32>>;

	i32 n;
	const Graph& g;
	std::vector<i32> tin, low, ids;

	explicit bcc(const Graph &t):
		n(static_cast<i32>(t.size())), g(t), tin(n, -1), low(n), ids(n, -1) {
		build();
	}

	bool is_bridge(i32 u, i32 v) {
		if (tin[u] > tin[v]) std::swap(u, v);
		return low[v] > tin[u];
	}

private:
	void build() {
		i32 timer = 0;
		const auto dfs = [&](auto &&self, i32 u, i32 t = -1) -> void {
			tin[u] = low[u] = timer++;
	 
			i32 cnt{};
			for (const i32 v : g[u]) {
				if (v == t && !cnt) {
					++cnt;
					continue;
				}
	 
				if (tin[v] != -1) {
					low[u] = std::min(low[u], tin[v]);
				} else {
					self(self, v, u);
					low[u] = std::min(low[u], low[v]);
				}
			}
		};
	 
		for (i32 u = 0; u < n; ++u) {
			if (tin[u] != -1) continue;
			dfs(dfs, u);
		}

		i32 group{};
		const auto dfs2 = [&](auto &&self, i32 u) -> void {
			ids[u] = group;

			for (const i32 v : g[u]) {
				if (ids[v] != -1 || is_bridge(u, v)) continue;
				self(self, v);
			}
		};

		for (i32 u = 0; u < n; ++u) {
			if (ids[u] != -1) continue;
			dfs2(dfs2, u);

			++group;
		}
	}
};

#endif // LIB_BCC_HPP
