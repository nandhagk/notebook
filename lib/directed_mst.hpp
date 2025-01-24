#ifndef LIB_DIRECTED_MST_HPP
#define LIB_DIRECTED_MST_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/rollback_dsu.hpp>
#include <lib/type_traits.hpp>

template <typename Graph>
inline std::vector<i32> directed_mst(const Graph &g, const i32 root) {
    using T = graph_weight_t<Graph>;

    const i32 n = static_cast<i32>(g.size());

    i32 m{};
    for (i32 u = 0; u < n; ++u) m += static_cast<i32>(g[u].size());

    std::vector<i32> seen(n, -1), path(n), queue(n), in(n, -1), heap(n, -1);
    std::vector<i32> from(m), to(m), left(m), right(m);
    std::vector<T> cost(m), lazy(m);

    const auto push = [&](i32 u) -> void {
        cost[u] += lazy[u];
        if (const i32 l = left[u]; ~l) lazy[l] += lazy[u];
        if (const i32 r = right[u]; ~r) lazy[r] += lazy[u];
        lazy[u] = 0;
    };

    const auto merge = [&](auto &&self, i32 u, i32 v) -> i32 {
        if (!~u || !~v) return ~u ? u : v;

        push(u);
        push(v);
        if (cost[u] > cost[v]) std::swap(u, v);

        right[u] = self(self, v, right[u]);
        std::swap(left[u], right[u]);
        return u;
    };

    const auto pop = [&](i32 u) -> i32 {
        push(u);
        return merge(merge, left[u], right[u]);
    };

    i32 id{};
    for (i32 u = 0; u < n; ++u) {
        for (const auto &[v, w] : g[u]) {
            from[id] = u;
            to[id] = v;
            cost[id] = w;
            left[id] = -1;
            right[id] = -1;
            heap[v] = merge(merge, heap[v], id);
            ++id;
        }
    }

    rollback_dsu dsu(n);
    seen[root] = root;

    std::vector<std::pair<i32, std::vector<i32>>> cycles;
    for (i32 s = 0; s < n; ++s) {
        i32 u = s, pos = 0, w = -1;
        while (!~seen[u]) {
            if (!~heap[u]) return {};

            push(heap[u]);
            const i32 e = heap[u];
            lazy[heap[u]] -= cost[e];
            heap[u] = pop(heap[u]);
            queue[pos] = e;
            path[pos++] = u;
            seen[u] = s;
            u = dsu.find(from[e]);
            if (seen[u] == s) {
                i32 cycle = -1;
                i32 end = pos;
                do {
                    w = path[--pos];
                    cycle = merge(merge, cycle, heap[w]);
                } while (dsu.merge(u, w));

                u = dsu.find(u);
                heap[u] = cycle;
                seen[u] = -1;
                cycles.emplace_back(u, std::vector<i32>(queue.begin() + pos, queue.begin() + end));
            }
        }

        for (i32 i = 0; i < pos; ++i) in[dsu.find(to[queue[i]])] = queue[i];
    }

    std::reverse(cycles.begin(), cycles.end());
    for (const auto &[u, comp] : cycles) {
        const i32 count = static_cast<i32>(comp.size()) - 1;
        dsu.rollback(count);

        const i32 inedge = in[u];
        for (const i32 e : comp) in[dsu.find(to[e])] = e;

        in[dsu.find(to[inedge])] = inedge;
    }

    std::vector<i32> par(n);
    for (i32 u = 0; u < n; ++u) par[u] = ~in[u] ? from[in[u]] : -1;

    return par;
}

#endif // LIB_DIRECTED_MST_HPP
