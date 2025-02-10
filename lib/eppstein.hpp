#ifndef LIB_EPPSTEIN_HPP
#define LIB_EPPSTEIN_HPP 1

#include <queue>
#include <vector>

#include <lib/csr_graph.hpp>
#include <lib/dijkstra.hpp>
#include <lib/prelude.hpp>

template <typename Key, typename Value>
struct leftist_heap {
    using self_t = leftist_heap<Key, Value>;

    i32 rank;
    Key key;
    Value value;
    self_t *left, *right;

    constexpr leftist_heap(i32 rank_, Key key_, Value value_, self_t *left_, self_t *right_)
        : rank{rank_}, key{key_}, value{value_}, left{left_}, right{right_} {}

    static self_t *insert(self_t *const a, const Key k, const Value v, std::deque<self_t> &alloc) {
        if (!a || k <= a->key) {
            alloc.emplace_back(1, k, v, a, nullptr);
            return &alloc.back();
        }

        auto l = a->left;
        auto r = insert(a->right, k, v, alloc);
        if (!l || r->rank > l->rank) std::swap(l, r);

        alloc.emplace_back(r ? r->rank + 1 : 0, a->key, a->value, l, r);
        return &alloc.back();
    }
};

template <typename Graph>
inline std::vector<graph_weight_t<Graph>> eppstein(const Graph &g, i32 s, i32 t, i32 k) {
    using W = graph_weight_t<Graph>;

    const auto r = g.reverse();
    const auto [d, prv] = dijkstra(r, t);

    if (d[s] == inf<W>) return {};

    const i32 n = static_cast<i32>(g.size());

    std::vector<std::pair<i32, simple_edge>> es;
    es.reserve(n);

    for (i32 u = 0; u < n; ++u)
        if (prv[u] != -1) es.emplace_back(prv[u], u);

    csr_graph z(n, es);

    using heap_t = leftist_heap<W, i32>;
    std::vector<heap_t *> h(n, nullptr);

    std::queue<i32> q;
    q.push(t);

    std::deque<heap_t> alloc;
    while (!q.empty()) {
        const i32 u = q.front();
        q.pop();

        bool seen = false;
        for (const auto &[v, w] : g[u]) {
            if (d[v] == inf<W>) continue;

            const auto c = w + d[v] - d[u];
            if (!seen && v == prv[u] && c == W(0)) {
                seen = true;
                continue;
            }

            h[u] = heap_t::insert(h[u], c, v, alloc);
        }

        for (const i32 v : z[u]) {
            h[v] = h[u];
            q.push(v);
        }
    }

    std::vector<W> ans{d[s]};
    ans.reserve(k);

    if (!h[s]) return ans;

    using Q = std::pair<W, heap_t *>;
    std::priority_queue<Q, std::vector<Q>, std::greater<Q>> pq;

    pq.emplace(d[s] + h[s]->key, h[s]);
    while (!pq.empty() && static_cast<i32>(ans.size()) < k) {
        const auto [cd, ch] = pq.top();
        pq.pop();

        ans.push_back(cd);
        if (h[ch->value]) pq.emplace(cd + h[ch->value]->key, h[ch->value]);
        if (ch->left) pq.emplace(cd + ch->left->key - ch->key, ch->left);
        if (ch->right) pq.emplace(cd + ch->right->key - ch->key, ch->right);
    }

    return ans;
}

#endif // LIB_EPPSTEIN_HPP
