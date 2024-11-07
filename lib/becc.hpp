#ifndef LIB_BECC_HPP
#define LIB_BECC_HPP 1

#include <vector>
#include <lib/prelude.hpp>

inline std::vector<i32> becc(const std::vector<std::vector<i32>> &g) {
	const i32 n = static_cast<i32>(g.size());

        i32 time{}, group{};
        std::vector<i32> tin(n, -1), low(n), ids(n, -1);

        const auto dfs = [&](auto &&self, i32 u, i32 t = -1) -> void {
                tin[u] = low[u] = time++;
 
                i32 cnt{};
                for (const i32 v : g[u]) {
                        if (v == t && !cnt++) continue;
 
                        if (tin[v] == -1) {
                                self(self, v, u);
                                low[u] = std::min(low[u], low[v]);
                        } else {
                                low[u] = std::min(low[u], tin[v]);
			}
                }
        };
 
        for (i32 u = 0; u < n; ++u) {
                if (tin[u] == -1) dfs(dfs, u);
	}

	const auto is_bridge = [&](i32 u, i32 v) {
		if (tin[u] > tin[v]) std::swap(u, v);
		return tin[u] < low[v];
	};

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

	return ids;
}

#endif // LIB_BECC_HPP
