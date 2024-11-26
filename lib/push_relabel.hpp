#ifndef LIB_PUSH_RELABEL_HPP
#define LIB_PUSH_RELABEL_HPP 1

#include <vector>
#include <algorithm>
#include <cassert>

#include <lib/prelude.hpp>

template <typename T> 
struct push_relabel {
        struct edge {
                i32 from, to;
                T cap, flow;
                i32 nxt;

                edge(i32 u, i32 v, T c, i32 nx): 
                        from(u), to(v), cap(c), flow(0), nxt(nx) {}
        };

        i32 n, max_level;
        std::vector<edge> es;
        std::vector<std::vector<i32>> lst;
        std::vector<i32> head, level, cnt;
        std::vector<T> excess;
        std::vector<bool> active;

        explicit push_relabel(i32 m) {
                build(m);
        }

        void build(i32 m) {
                n = m;
                lst.resize(n);
                head.assign(n, -1);
        }

        void add_edge(i32 from, i32 to, T cap) {
                const i32 id = static_cast<i32>(es.size());
                es.emplace_back(from, to, cap, head[from]);
                head[from] = id;
                es.emplace_back(to, from, 0, head[to]);
                head[to] = id + 1;
        }

        T max_flow(i32 s, i32 t) {
                for (auto &&e : es) e.flow = 0;
                for (auto &&l : lst) l.clear();

                max_level = 0;
                level.assign(n, 0);
                excess.assign(n, 0);
                cnt.assign(n + 1, 0);
                active.assign(n, false);

                for (i32 i = head[s]; i != -1; i = es[i].nxt) excess[s] += es[i].cap;
                cnt[0] = n;
                enqueue(s);
                active[t] = true;
                while (max_level >= 0) {
                        if (lst[max_level].empty()) {
                                --max_level;
                                continue;
                        }

                        const i32 u = lst[max_level].back();
                        lst[max_level].pop_back();

                        active[u] = false;
                        for (i32 i = head[u]; i != -1 && excess[u] > 0; i = es[i].nxt) {
                                const auto d = std::min(excess[u], es[i].cap - es[i].flow);
                                if (d > 0 && level[u] == level[es[i].to] + 1) {
                                        es[i].flow += d; es[i ^ 1].flow -= d;
                                        excess[es[i].to] += d; excess[u] -= d;
                                        enqueue(es[i].to);
                                }
                        }

                        if (excess[u] == 0) continue;

                        if (cnt[level[u]] == 1) {
                                const i32 k = level[u];
                                for (i32 v = 0; v < n; ++v) {
                                        if (level[v] < k) continue;

                                        --cnt[level[v]];
                                        level[v] = n;

                                        ++cnt[level[v]];
                                        enqueue(v);
                                }
                        } else {
                                --cnt[level[u]];
                                level[u] = n;

                                for (i32 i = head[u]; i != -1; i = es[i].nxt) {
                                        if (es[i].cap == es[i].flow) continue;
                                        level[u] = std::min(level[u], level[es[i].to] + 1);
                                }

                                ++cnt[level[u]];
                                enqueue(u);
                        }
                }

                return excess[t];
        }

private:
        void enqueue(i32 u) {
                if (!active[u] && excess[u] > 0 && level[u] < n) {
                        active[u] = true;
                        lst[level[u]].push_back(u);
                        max_level = std::max(max_level, level[u]);
                }
        }
};

#endif // LIB_PUSH_RELABEL_HPP
