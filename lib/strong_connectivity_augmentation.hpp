#ifndef LIB_STRONG_CONNECTIVITY_AUGMENTATION_HPP
#define LIB_STRONG_CONNECTIVITY_AUGMENTATION_HPP 1

#include <vector>

#include <lib/prelude.hpp>
#include <lib/scc.hpp>
#include <lib/topological_sort.hpp>

// https://codeforces.com/blog/entry/80391?#comment-667198
template <typename Graph>
inline std::vector<std::pair<i32, i32>> strong_connectivity_augmentation(const Graph &h) {
    assert(topological_sort(h));

    const i32 k = static_cast<i32>(h.size());
    if (k == 1) return {};

    std::vector<bool> zero_in(k, true);
    for (i32 u = 0; u < k; ++u)
        for (const i32 v : h[u]) zero_in[v] = false;

    std::vector<bool> seen(k, false);
    const auto dfs = [&](auto &&self, i32 u) -> i32 {
        bool flag = false;
        for (const i32 v : h[u]) {
            flag = true;
            if (seen[v]) continue;

            seen[v] = true;
            if (const i32 zero_out = self(self, v); zero_out != -1) return zero_out;
        }

        return flag ? -1 : u;
    };

    std::vector<std::pair<i32, i32>> es;
    std::vector<i32> in_unused;

    for (i32 u = 0; u < k; ++u) {
        if (!zero_in[u]) continue;

        seen[u] = true;
        if (const i32 zero_out = dfs(dfs, u); zero_out != -1)
            es.emplace_back(zero_out, u);
        else
            in_unused.push_back(u);
    }

    for (i32 i = 1; i < static_cast<i32>(es.size()); ++i) std::swap(es[i].first, es[i - 1].first);

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
}

#endif // LIB_STRONG_CONNECTIVITY_AUGMENTATION_HPP
