#ifndef LIB_SCC_HPP
#define LIB_SCC_HPP 1

#include <vector>
#include <lib/prelude.hpp>

inline std::vector<i32> scc(const std::vector<std::vector<i32>> &g) {
	const i32 n = static_cast<i32>(g.size());

	std::vector<i32> visited, low(n), tin(n, -1), ids(n);
	visited.reserve(n);

	i32 timer{}, group{};
	const auto dfs = [&](auto &&self, i32 u) -> void {
		low[u] = tin[u] = timer++;
		visited.push_back(u);

		for (const i32 v : g[u]) {
			if (tin[v] == -1) {
				self(self, v);
				low[u] = std::min(low[u], low[v]);
			} else {
				low[u] = std::min(low[u], tin[v]);
			}
		}

		if (low[u] == tin[u]) {
			for (;;) {
				const i32 v = visited.back();
				visited.pop_back();

				tin[v] = n;
				ids[v] = group;
				
				if (u == v) break;
			}

			++group;
		}
	};

	for (i32 u = 0; u < n; ++u) {
		if (tin[u] != -1) continue;
		dfs(dfs, u);
	}

	for (auto& id : ids) id = group - 1 - id;
	return ids;
}

#endif // LIB_SCC_HPP
