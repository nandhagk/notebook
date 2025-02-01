#ifndef LIB_ROOTED_TREE_ISOMORPHISM_HPP
#define LIB_ROOTED_TREE_ISOMORPHISM_HPP 1

#include <algorithm>
#include <map>
#include <vector>

#include <lib/prelude.hpp>

template <typename Map, typename Graph>
inline std::vector<i32> rooted_tree_isomorphism(Map &k, const Graph &g, i32 root = 0) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<i32> out(n);
    const auto dfs = [&](auto &&self, i32 u, i32 t = -1) -> void {
        std::vector<i32> cs;
        cs.reserve(g[u].size());

        for (const i32 v : g[u]) {
            if (v == t) continue;

            self(self, v, u);
            cs.push_back(out[v]);
        }

        std::sort(cs.begin(), cs.end());
        const auto &[it, _] = k.insert({std::move(cs), static_cast<i32>(k.size())});

        out[u] = it->second;
    };

    dfs(dfs, root);
    return out;
}

template <typename Graph>
inline std::vector<i32> rooted_tree_isomorphism(const Graph &g, i32 root = 0) {
    std::map<std::vector<i32>, i32> k;
    return rooted_tree_isomorphism(k, g, root);
}

#endif // LIB_ROOTED_TREE_ISOMORPHISM_HPP
