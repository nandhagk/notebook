#ifndef LIB_AM_TREE_HPP
#define LIB_AM_TREE_HPP 1

#include <algorithm>
#include <cassert>
#include <limits>
#include <optional>
#include <utility>
#include <vector>

#include <lib/prelude.hpp>

// Fast Incremental minimum spanning tree
// based on a direct implementation of Anti-Monopoly tree
//
// ## Reference
// - Xiangyun Ding, Yan Gu, Yihan Sun.
// "New Algorithms for Incremental Minimum Spanning Trees and Temporal Graph Applications".
// [https://arxiv.org/abs/2504.04619]

template <typename T, bool LINK_BY_STITCH = true>
struct am_tree {
    struct node {
        i32 parent;
        i32 size;
        T weight;
    };

    using insert_result = std::optional<std::optional<T>>;

    std::vector<node> us;

    am_tree() {}

    explicit am_tree(i32 n) {
        build(n);
    }

    void build(i32 n) {
        us.assign(n, {UNSET, 1, T{}});
    }

    insert_result insert(i32 u, i32 v, T w) {
        if (u == v) return std::nullopt;

        upward_calibrate(u);
        upward_calibrate(v);

        if constexpr (LINK_BY_STITCH)
            return link_by_stitch(u, v, w);
        else
            return link_by_perch(u, v, w);
    }

    std::optional<T> max_path(i32 u, i32 v) {
        if (u == v) return std::nullopt;

        upward_calibrate(u);
        upward_calibrate(v);

        std::optional<T> res = std::nullopt;

        for (;;) {
            if (us[u].size > us[v].size) std::swap(u, v);
            if (!res.has_value() || us[u].weight > *res) res = us[u].weight;

            u = us[u].parent;
            if (u == UNSET) return std::nullopt;
            if (u == v) return res;
        }
    }

private:
    static constexpr i32 UNSET = -1;

    void promote(i32 u) {
        i32 p = us[u].parent;
        i32 g = us[p].parent;

        T wu = us[u].weight;
        T wp = us[p].weight;

        if (wu >= wp && g != UNSET) {
            us[u].parent = g;
            us[p].size -= us[u].size;
        } else {
            us[u].parent = g;
            us[p].parent = u;
            us[u].weight = wp;
            us[p].weight = wu;
            us[p].size -= us[u].size;
            us[u].size += us[p].size;
        }
    }

    void perch(i32 u) {
        while (us[u].parent != UNSET) promote(u);
    }

    insert_result link_by_perch(i32 u, i32 v, T w) {
        assert(u != v);

        perch(u);
        perch(v);

        if (us[u].parent == v) {
            T wo = us[u].weight;
            if (w < wo) {
                us[u].weight = w;
                return std::optional<T>(wo);
            } else {
                return std::nullopt;
            }
        } else {
            us[u].parent = v;
            us[u].weight = w;
            us[v].size += us[u].size;
            return std::optional<T>(std::nullopt);
        }
    }

    insert_result cut_max_path(i32 u, i32 v, T w) {
        assert(u != v);

        std::optional<std::pair<T, i32>> mx = std::nullopt;

        for (;;) {
            if (us[u].size > us[v].size) std::swap(u, v);

            i32 p = us[u].parent;
            if (p == UNSET) return std::optional<T>(std::nullopt);

            if (!mx || us[u].weight > mx->first) mx = {us[u].weight, u};

            u = p;
            if (u == v) {
                auto [mw, t] = *mx;
                if (w >= mw) return std::nullopt;

                i32 pt = us[t].parent;
                us[t].parent = UNSET;
                us[t].weight = T{};
                i32 ds = us[t].size;

                i32 it = pt;
                while (it != UNSET) {
                    us[it].size -= ds;
                    it = us[it].parent;
                }

                return std::optional<T>(mw);
            }
        }
    }

    insert_result link_by_stitch(i32 u, i32 v, T w) {
        assert(u != v);

        insert_result res = cut_max_path(u, v, w);
        if (!res.has_value()) return std::nullopt;

        for (i32 du = 0, dv = 0;;) {
            while (us[u].parent != UNSET && w >= us[u].weight) {
                u = us[u].parent;
                us[u].size += du;
            }

            while (us[v].parent != UNSET && w >= us[v].weight) {
                v = us[v].parent;
                us[v].size += dv;
            }

            if (us[u].size > us[v].size) {
                std::swap(u, v);
                std::swap(du, dv);
            }

            i32 su = us[u].size;
            du -= su;
            dv += su;
            us[v].size += su;

            std::swap(us[u].weight, w);

            i32 op = us[u].parent;
            us[u].parent = v;
            u = op;

            if (u == UNSET) {
                for (;;) {
                    v = us[v].parent;
                    if (v == UNSET) return res;
                    us[v].size += dv;
                }
            }
            us[u].size += du;
        }
    }

    void upward_calibrate(i32 u) {
        for (i32 p = us[u].parent; p != UNSET; p = us[u].parent)
            if (us[u].size * 3 / 2 > us[p].size)
                promote(u);
            else
                u = p;
    }
};

#endif // LIB_AM_TREE_HPP
