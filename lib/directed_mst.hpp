#ifndef LIB_DIRECTED_MST_HPP
#define LIB_DIRECTED_MST_HPP 1

#include <vector>
#include <cassert>

#include <lib/prelude.hpp>
#include <lib/rollback_dsu.hpp>

template <typename T> 
struct directed_mst {
        explicit directed_mst(i32 m) : n(m), heap(n, -1) {}

        void add_edge(i32 u, i32 v, T w) {
                assert(0 <= u && u < n);
                assert(0 <= v && v < n);

                const i32 id = static_cast<i32>(from.size());
                from.push_back(u);
                to.push_back(v);
                cost.push_back(w);
                left.push_back(-1);
                right.push_back(-1);
                lazy.push_back(T(0));

                heap[v] = merge(heap[v], id);
        }

        std::pair<T, std::vector<i32>> run(i32 root) {
                rollback_dsu dsu(n);
                T result{};

                std::vector<i32> seen(n, -1), path(n), queue(n), in(n, -1);
                seen[root] = root;

                std::vector<std::pair<i32, std::vector<i32>>> cycles;
                for (i32 s = 0; s < n; ++s) {
                        i32 u = s, pos = 0, w = -1;
                        while (!~seen[u]) {
                                if (!~heap[u]) return {-1, {}};

                                push(heap[u]);
                                const i32 e = heap[u];
                                result += cost[e];
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
                                                cycle = merge(cycle, heap[w]);
                                        } while (dsu.merge(u, w));

                                        u = dsu.find(u);
                                        heap[u] = cycle;
                                        seen[u] = -1;
                                        cycles.emplace_back(u,
                                                std::vector<i32>(queue.begin() + pos,
                                                                 queue.begin() + end));
                                }
                        }

                        for (i32 i = 0; i < pos; ++i) in[dsu.find(to[queue[i]])] = queue[i];
                }

                for (auto it = cycles.rbegin(); it != cycles.rend(); ++it) {
                        const auto &[u, comp] = *it;
                        const i32 count = static_cast<i32>(comp.size()) - 1;
                        dsu.rollback(count);

                        i32 inedge = in[u];
                        for (const i32 e : comp) in[dsu.find(to[e])] = e;

                        in[dsu.find(to[inedge])] = inedge;
                }

                std::vector<i32> parent;
                parent.reserve(n);

                for (i32 i : in) parent.push_back(~i ? from[i] : -1);
                return {result, parent};
        }

private:
        void push(i32 u) {
                cost[u] += lazy[u];
                if (const i32 l = left[u]; ~l) lazy[l] += lazy[u];
                if (const i32 r = right[u]; ~r) lazy[r] += lazy[u];
                lazy[u] = 0;
        }

        i32 merge(i32 u, i32 v) {
                if (!~u || !~v) return ~u ? u : v;

                push(u);
                push(v);
                if (cost[u] > cost[v]) std::swap(u, v);

                right[u] = merge(v, right[u]);
                std::swap(left[u], right[u]);
                return u;
        }

        i32 pop(i32 u) {
                push(u);
                return merge(left[u], right[u]);
        }

        i32 n;
        std::vector<i32> from, to, left, right, heap;
        std::vector<T> cost, lazy;
};

#endif // LIB_DIRECTED_MST_HPP
