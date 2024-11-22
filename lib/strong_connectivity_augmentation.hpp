#ifndef LIB_STRONG_CONNECTIVITY_AUGMENTATION_HPP
#define LIB_STRONG_CONNECTIVITY_AUGMENTATION_HPP 1

#include <vector>

#include <lib/prelude.hpp>
#include <lib/scc.hpp>

// https://codeforces.com/blog/entry/80391?#comment-667198
inline std::vector<std::pair<i32, i32>> strong_connectivity_augmentation(
	const std::vector<std::vector<i32>> &g, const std::vector<i32> &ids) {
	const auto h = scc_dag(g, ids);

	const i32 n = static_cast<i32>(g.size());
	const i32 k = *std::max_element(ids.begin(), ids.end()) + 1;

	if (k == 1) return {};

	std::vector<bool> zero_in(k, true); 
	for (i32 u = 0; u < k; ++u) {
		for (const i32 v : h[u]) zero_in[v] = false;
	}

	std::vector<bool> seen(k, false);
	const auto dfs = [&](auto &&self, i32 u) -> i32 {
		if (h[u].empty()) return u;

		for (const i32 v : h[u]) {
			if (seen[v]) continue;

			seen[v] = true;
			if (const i32 zero_out = self(self, v); zero_out != -1) return zero_out; 
		}

		return -1;
	};

	std::vector<std::pair<i32, i32>> es;
	std::vector<i32> in_unused;

	for (i32 u = 0; u < k; ++u) {
		if (!zero_in[u]) continue;

		seen[u] = true;
		if (const i32 zero_out = dfs(dfs, u); zero_out != -1) {
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

	for (auto &&[u, v] : es) {
		u = rids[u];
		v = rids[v];
	}

	return es;
}

#endif // LIB_STRONG_CONNECTIVITY_AUGMENTATION_HPP