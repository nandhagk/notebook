#ifndef LIB_SCC_HPP
#define LIB_SCC_HPP 1

#include <vector>
#include <algorithm>
#include <lib/prelude.hpp>

inline std::vector<i32> scc(const std::vector<std::vector<i32>> &g) {
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
	return ids;
}

inline std::vector<std::pair<i32, i32>> make_strongly_connected(
	const std::vector<std::vector<i32>> &g, const std::vector<i32> &ids) {
	const i32 n = static_cast<i32>(g.size());
        const i32 k = *std::max_element(ids.begin(), ids.end()) + 1;

        if (k == 1) return {};

        std::vector<std::vector<i32>> h(k);
        std::vector<bool> zero_in(k, true); 

        for (i32 u = 0; u < n; ++u) {
                for (const i32 v : g[u]) {
                        if (ids[u] == ids[v]) continue;

                        h[ids[u]].push_back(ids[v]);
                        zero_in[ids[v]] = false;
                }
        }

        std::vector<bool> seen(k, false);
        const auto dfs = [&](auto &&self, i32 u) -> i32 {
                if (h[u].empty()) return u;

                for (const i32 v : h[u]) {
			if (seen[v]) continue;

			seen[v] = true;
			const i32 zero_out = self(self, v);
			if (zero_out != -1) return zero_out; 
                }

                return -1;
        };

	std::vector<std::pair<i32, i32>> es;
        std::vector<i32> in_unused;
        
        for (i32 u = 0; u < k; ++u) {
		if (!zero_in[u]) continue;

		seen[u] = true;
		const i32 zero_out = dfs(dfs, u);
		if (zero_out != -1) {
			es.emplace_back(zero_out, u);
		} else {
			in_unused.push_back(u);
		}
        }

        for (i32 i = 1; i < static_cast<i32>(es.size()); ++i) {
                std::swap(es[i].first, es[i - 1].first);
        }

        for (i32 u = 0; u < k; ++u) {
		if (!h[u].empty() || seen[u]) continue;

		if (!in_unused.empty()) {
			es.emplace_back(u, in_unused.back());
			in_unused.pop_back();
		} else {
			es.emplace_back(u, 0);
		}
        }

        for (const i32 u : in_unused) es.emplace_back(k - 1, u);

        std::vector<i32> rids(k);
        for (i32 u = 0; u < n; ++u) rids[ids[u]] = u;

        for (auto &[u, v] : es) {
                u = rids[u];
                v = rids[v];
        }

	return es;
}

#endif // LIB_SCC_HPP
