#ifndef LIB_SCC_HPP
#define LIB_SCC_HPP 1

#include <vector>
#include <algorithm>

#include <lib/prelude.hpp>
#include <lib/csr_graph.hpp>
#include <lib/dense_graph.hpp>

template <typename Graph>
inline std::pair<i32, std::vector<i32>> scc(const Graph &g) {
	const i32 n = static_cast<i32>(g.size());

	std::vector<i32> seen, low(n), tin(n, -1), ids(n);
	seen.reserve(n);

	i32 time{}, group{};
	const auto dfs = [&](auto &&self, i32 u) -> void {
		tin[u] = low[u] = time++;
		seen.push_back(u);

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
				const i32 v = seen.back();
				seen.pop_back();

				tin[v] = n;
				ids[v] = group;
				
				if (u == v) break;
			}

			++group;
		}
	};

	for (i32 u = 0; u < n; ++u) {
		if (tin[u] == -1) dfs(dfs, u);
	}

	for (auto& id : ids) id = group - 1 - id;
	return {group, std::move(ids)};
}

template <typename Graph>
inline csr_graph<simple_edge> scc_dag(const Graph &g, const std::vector<i32> &ids) {
	const i32 n = static_cast<i32>(g.size());
	const i32 k = *std::max_element(ids.begin(), ids.end()) + 1;

	std::vector<std::pair<i32, simple_edge>> es;
	es.reserve(g.m);

	for (i32 u = 0; u < n; ++u) {
		for (const i32 v : g[u]) {
			if (ids[u] == ids[v]) continue;
			es.emplace_back(ids[u], ids[v]);
		}
	}

	std::sort(es.begin(), es.end());
	es.erase(std::unique(es.begin(), es.end()), es.end());

	return csr_graph(k, es);
}

inline dense_graph scc_dag(const dense_graph &g, const std::vector<i32> &ids) {
	const i32 n = static_cast<i32>(g.size());
	const i32 k = *std::max_element(ids.begin(), ids.end()) + 1;

	dense_graph h(k);
	for (i32 u = 0; u < n; ++u) {
		for (const i32 v : g[u]) {
			if (ids[u] == ids[v]) continue;
			h.add_edge(ids[u], ids[v]);
		}
	}
	
	return h;
}

#endif // LIB_SCC_HPP
