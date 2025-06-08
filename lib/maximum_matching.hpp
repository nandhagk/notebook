#ifndef LIB_MAXIMUM_MATCHING_HPP
#define LIB_MAXIMUM_MATCHING_HPP 1

#include <cassert>
#include <vector>

#include <lib/csr_graph.hpp>
#include <lib/prelude.hpp>

class maximum_matching {
    /*
     * Maximum Cardinality Matching in General UndirectedGraphs.
     * - O(\sqrt{n} m \log_{\max\{2, 1 + m/n\}} n) time
     * - O(n + m) space
     *
     * Note: internally each vertex is 1-indexed.
     * Ref:
     * Harold N. Gabow,
     * "The Weighted Matching Approach to Maximum Cardinality Matching" (2017)
     * (https://arxiv.org/abs/1703.03998)
     */

private:
    enum label {
        kInner = -1, // should be < 0
        kFree = 0    // should be 0
    };

    struct link {
        i32 from, to;
    };

    struct log {
        i32 v, par;
    };

    struct linked_list {
        linked_list() {}
        linked_list(i32 n, i32 m)
            : N(n), next(m) {
            clear();
        }

        void clear() {
            head.assign(N, -1);
        }

        void push(i32 h, i32 u) {
            next[u] = head[h], head[h] = u;
        }

        i32 N;
        std::vector<i32> head, next;
    };

    template <typename T>
    struct queue {
        queue() {}
        explicit queue(i32 N)
            : qh(0), qt(0), data(N) {}

        T operator[](i32 i) const {
            return data[i];
        }

        void enqueue(i32 u) {
            data[qt++] = u;
        }

        i32 dequeue() {
            return data[qh++];
        }

        bool empty() const {
            return qh == qt;
        }

        void clear() {
            qh = qt = 0;
        }

        i32 size() const {
            return qt;
        }

        i32 qh, qt;
        std::vector<T> data;
    };

    struct disjoint_set_union {
        disjoint_set_union() {}
        explicit disjoint_set_union(i32 N)
            : par(N) {
            for (i32 i = 0; i < N; ++i) par[i] = i;
        }

        i32 find(i32 u) {
            return par[u] == u ? u : (par[u] = find(par[u]));
        }

        void merge(i32 u, i32 v) {
            u = find(u), v = find(v);
            if (u != v) par[v] = u;
        }

        std::vector<i32> par;
    };

public:
    template <typename UndirectedGraph>
    explicit maximum_matching(const UndirectedGraph &g)
        : N(static_cast<i32>(g.size())), NH(N >> 1), ofs(N + 2, 0) {
        for (i32 u = 0; u < N; ++u) ofs[u + 2] = static_cast<i32>(g[u].size());
        for (i32 i = 1; i <= N + 1; ++i) ofs[i] += ofs[i - 1];

        edges.reserve(ofs.back());
        for (i32 u = 0; u < N; ++u)
            for (const i32 v : g[u]) edges.emplace_back(u + 1, v + 1);
    }

    i32 matching() {
        initialize();

        i32 match = 0;
        while (match * 2 + 1 < N) {
            reset_count();
            bool has_augmenting_path = do_edmonds_search();
            if (!has_augmenting_path) break;
            match += find_maximal();
            clear();
        }

        return match;
    }

    std::vector<std::pair<i32, simple_edge>> get_matching_edges() {
        std::vector<std::pair<i32, simple_edge>> ans;
        for (i32 i = 1; i <= N; ++i)
            if (mate[i] > i) ans.emplace_back(i - 1, mate[i] - 1);

        return ans;
    }

private:
    void reset_count() {
        time_current = 0;
        time_augment = inf<i32>;
        contract_count = 0;
        outer_id = 1;
        dsu_changelog_size = dsu_changelog_last = 0;
    }

    void clear() {
        que.clear();
        for (i32 u = 1; u <= N; ++u) potential[u] = 1;
        for (i32 u = 1; u <= N; ++u) dsu.par[u] = u;
        for (i32 t = time_current; t <= N / 2; ++t) list.head[t] = -1;
        for (i32 u = 1; u <= N; ++u) blossom.head[u] = -1;
    }

    // first phase

    inline void grow(i32 x, i32 y, i32 z) {
        label[y] = kInner;
        potential[y] = time_current; // visited time
        link[z] = {x, y};
        label[z] = label[x];
        potential[z] = time_current + 1;
        que.enqueue(z);
    }

    void contract(i32 x, i32 y) {
        i32 bx = dsu.find(x), by = dsu.find(y);
        const i32 h = -(++contract_count) + kInner;
        label[mate[bx]] = label[mate[by]] = h;
        i32 lca = -1;
        while (1) {
            if (mate[by] != 0) std::swap(bx, by);
            bx = lca = dsu.find(link[bx].from);
            if (label[mate[bx]] == h) break;
            label[mate[bx]] = h;
        }
        for (auto bv : {dsu.par[x], dsu.par[y]}) {
            for (; bv != lca; bv = dsu.par[link[bv].from]) {
                i32 mv = mate[bv];
                link[mv] = {x, y};
                label[mv] = label[x];
                potential[mv] = 1 + (time_current - potential[mv]) + time_current;
                que.enqueue(mv);
                dsu.par[bv] = dsu.par[mv] = lca;
                dsu_changelog[dsu_changelog_last++] = {bv, lca};
                dsu_changelog[dsu_changelog_last++] = {mv, lca};
            }
        }
    }

    bool find_augmenting_path() {
        while (!que.empty()) {
            i32 x = que.dequeue(), lx = label[x], px = potential[x], bx = dsu.find(x);
            for (i32 eid = ofs[x]; eid < ofs[x + 1]; ++eid) {
                i32 y = edges[eid].second;
                if (label[y] > 0) { // outer blossom/vertex
                    i32 time_next = (px + potential[y]) >> 1;
                    if (lx != label[y]) {
                        if (time_next == time_current) return true;
                        time_augment = std::min(time_next, time_augment);
                    } else {
                        if (bx == dsu.find(y)) continue;
                        if (time_next == time_current) {
                            contract(x, y);
                            bx = dsu.find(x);
                        } else if (time_next <= NH) {
                            list.push(time_next, eid);
                        }
                    }
                } else if (label[y] == kFree) { // free vertex
                    i32 time_next = px + 1;
                    if (time_next == time_current)
                        grow(x, y, mate[y]);
                    else if (time_next <= NH)
                        list.push(time_next, eid);
                }
            }
        }
        return false;
    }

    bool adjust_dual_variables() {
        // Return true if the current matching is maximum.
        const i32 time_lim = std::min(NH + 1, time_augment);
        for (++time_current; time_current <= time_lim; ++time_current) {
            dsu_changelog_size = dsu_changelog_last;
            if (time_current == time_lim) break;
            bool updated = false;
            for (i32 h = list.head[time_current]; h >= 0; h = list.next[h]) {
                auto &e = edges[h];
                i32 x = e.first, y = e.second;
                if (label[y] > 0) {
                    // Case: outer -- (free => inner => outer)
                    if (potential[x] + potential[y] != (time_current << 1)) continue;
                    if (dsu.find(x) == dsu.find(y)) continue;
                    if (label[x] != label[y]) {
                        time_augment = time_current;
                        return false;
                    }
                    contract(x, y);
                    updated = true;
                } else if (label[y] == kFree) {
                    grow(x, y, mate[y]);
                    updated = true;
                }
            }
            list.head[time_current] = -1;
            if (updated) return false;
        }
        return time_current > NH;
    }

    bool do_edmonds_search() {
        label[0] = kFree;
        for (i32 u = 1; u <= N; ++u) {
            if (mate[u] == 0) {
                que.enqueue(u);
                label[u] = u; // component id
            } else {
                label[u] = kFree;
            }
        }
        while (1) {
            if (find_augmenting_path()) break;
            bool maximum = adjust_dual_variables();
            if (maximum) return false;
            if (time_current == time_augment) break;
        }
        for (i32 u = 1; u <= N; ++u)
            if (label[u] > 0)
                potential[u] -= time_current;
            else if (label[u] < 0)
                potential[u] = 1 + (time_current - potential[u]);
        return true;
    }

    // second phase

    void rematch(i32 v, i32 w) {
        i32 t = mate[v];
        mate[v] = w;
        if (mate[t] != v) return;
        if (link[v].to == dsu.find(link[v].to)) {
            mate[t] = link[v].from;
            rematch(mate[t], t);
        } else {
            i32 x = link[v].from, y = link[v].to;
            rematch(x, y);
            rematch(y, x);
        }
    }

    bool dfs_augment(i32 x, i32 bx) {
        i32 px = potential[x], lx = label[bx];
        for (i32 eid = ofs[x]; eid < ofs[x + 1]; ++eid) {
            i32 y = edges[eid].second;
            if (px + potential[y] != 0) continue;
            i32 by = dsu.find(y), ly = label[by];
            if (ly > 0) { // outer
                if (lx >= ly) continue;
                i32 stack_beg = stack_last;
                for (i32 bv = by; bv != bx; bv = dsu.find(link[bv].from)) {
                    i32 bw = dsu.find(mate[bv]);
                    stack[stack_last++] = bw;
                    link[bw] = {x, y};
                    dsu.par[bv] = dsu.par[bw] = bx;
                }
                while (stack_last > stack_beg) {
                    i32 bv = stack[--stack_last];
                    for (i32 v = blossom.head[bv]; v >= 0; v = blossom.next[v]) {
                        if (!dfs_augment(v, bx)) continue;
                        stack_last = stack_beg;
                        return true;
                    }
                }
            } else if (ly == kFree) {
                label[by] = kInner;
                i32 z = mate[by];
                if (z == 0) {
                    rematch(x, y);
                    rematch(y, x);
                    return true;
                }
                i32 bz = dsu.find(z);
                link[bz] = {x, y};
                label[bz] = outer_id++;
                for (i32 v = blossom.head[bz]; v >= 0; v = blossom.next[v])
                    if (dfs_augment(v, bz)) return true;
            }
        }
        return false;
    }

    i32 find_maximal() {
        // discard blossoms whose potential is 0.
        for (i32 u = 1; u <= N; ++u) dsu.par[u] = u;
        for (i32 i = 0; i < dsu_changelog_size; ++i) dsu.par[dsu_changelog[i].v] = dsu_changelog[i].par;
        for (i32 u = 1; u <= N; ++u) {
            label[u] = kFree;
            blossom.push(dsu.find(u), u);
        }
        i32 ret = 0;
        for (i32 u = 1; u <= N; ++u) {
            if (!mate[u]) {
                i32 bu = dsu.par[u];
                if (label[bu] != kFree) continue;
                label[bu] = outer_id++;
                for (i32 v = blossom.head[bu]; v >= 0; v = blossom.next[v]) {
                    if (!dfs_augment(v, bu)) continue;
                    ret += 1;
                    break;
                }
            }
        }
        assert(ret >= 1);
        return ret;
    }

    void initialize() {
        que = queue<i32>(N);

        mate.assign(N + 1, 0);
        potential.assign(N + 1, 1);
        label.assign(N + 1, kFree);
        link.assign(N + 1, {0, 0});

        dsu_changelog.resize(N);

        dsu = disjoint_set_union(N + 1);
        list = linked_list(NH + 1, static_cast<i32>(edges.size()));

        blossom = linked_list(N + 1, N + 1);
        stack.resize(N);
        stack_last = 0;
    }

private:
    const i32 N, NH;
    std::vector<i32> ofs;
    std::vector<std::pair<i32, simple_edge>> edges;

    queue<i32> que;

    std::vector<i32> mate, potential;
    std::vector<i32> label;
    std::vector<link> link;

    std::vector<log> dsu_changelog;
    i32 dsu_changelog_last, dsu_changelog_size;

    disjoint_set_union dsu;
    linked_list list, blossom;
    std::vector<i32> stack;
    i32 stack_last;

    i32 time_current, time_augment;
    i32 contract_count, outer_id;
};

#endif // LIB_MAXIMUM_MATCHING_HPP
