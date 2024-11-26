#ifndef LIB_PUSH_RELABEL_HPP
#define LIB_PUSH_RELABEL_HPP 1

#include <vector>
#include <algorithm>
#include <cassert>

#include <lib/prelude.hpp>

template <typename T>
inline T push_relabel(const std::vector<std::vector<std::pair<i32, T>>> &g, i32 s, i32 t) {
        const i32 n = static_cast<i32>(g.size());

        i32 k{};
        std::vector<std::vector<std::tuple<i32, i32, T>>> h(n);
        for (i32 u = 0; u < n; ++u) {
                for (const auto &[v, cap] : g[u]) {
                        h[u].emplace_back(v, k++, cap);
                        h[v].emplace_back(u, k++, 0);
                }
        }

        std::vector<T> flow(k), excess(n);
        std::vector<i32> lvl(n), cnt(n + 1);
        std::vector<std::vector<i32>> lst(n);
        std::vector<bool> active(n, false);

        i32 max{};
        for (const auto &[_, cap] : g[s]) excess[s] += cap;

        const auto enqueue = [&](i32 u) -> void {
                if (active[u] || excess[u] == 0 || lvl[u] == n) return;

                active[u] = true;
                lst[lvl[u]].push_back(u);
                max = std::max(max, lvl[u]);
        };

        cnt[0] = n;
        active[t] = true;

        enqueue(s);
        while (max >= 0) {
                if (lst[max].empty()) {
                        --max;
                        continue;
                }

                const i32 u = lst[max].back();
                lst[max].pop_back();

                active[u] = false;
                for (const auto &[v, i, cap] : h[u]) {
                        const auto d = std::min(excess[u], cap - flow[i]);
                        if (d > 0 && lvl[u] == lvl[v] + 1) {
                                flow[i] += d;
                                flow[i ^ 1] -= d;

                                excess[v] += d;
                                excess[u] -= d;

                                enqueue(v);
                        }
                }

                if (excess[u] == 0) continue;

                if (cnt[lvl[u]] == 1) {
                        const i32 l = lvl[u];
                        for (i32 v = 0; v < n; ++v) {
                                if (lvl[v] < l) continue;

                                --cnt[lvl[v]];
                                lvl[v] = n;

                                ++cnt[lvl[v]];
                                enqueue(v);
                        }
                } else {
                        --cnt[lvl[u]];
                        lvl[u] = n;

                        for (const auto &[v, i, cap] : h[u]) {
                                if (cap == flow[i]) continue;
                                lvl[u] = std::min(lvl[u], lvl[v] + 1);
                        }

                        ++cnt[lvl[u]];
                        enqueue(u);
                }
        }

        for (i32 u = 0; u < n; ++u) {
                for (const auto &[v, i, cap] : h[u]) {
                        debug(u, v, cap - flow[i]);
                }
        }

        return excess[t];
};

#endif // LIB_PUSH_RELABEL_HPP
