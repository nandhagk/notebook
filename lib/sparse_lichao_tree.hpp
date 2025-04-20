#ifndef LIB_SPARSE_LICHAO_TREE_HPP
#define LIB_SPARSE_LICHAO_TREE_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>

template <typename L, bool MIN = true, bool PERSIST = false>
struct sparse_lichao_tree {
    using T = typename L::ValueT;

    std::vector<L> d;

    struct node {
        i32 i;
        node *l, *r;

        node()
            : i{-1}, l{nullptr}, r{nullptr} {}
    };

    using np = node *;

    np pool;

    i32 n, pid;
    T xmin, xmax;

    sparse_lichao_tree()
        : pool{nullptr} {}

    sparse_lichao_tree(T mi, T ma, i32 m)
        : n{m}, pid{0}, xmin{mi}, xmax{ma} {
        pool = new node[n];
    }

    ~sparse_lichao_tree() {
        reset();
    }

    void reset() {
        pid = 0;
        d.clear();
        delete[] pool;
    }

    np make_root() {
        return nullptr;
    }

    np make_node() {
        assert(pid < n);

        return &(pool[pid++] = node());
    }

    np add_line(np root, L f) {
        const i32 i = static_cast<i32>(d.size());
        d.push_back(f);

        if (root == nullptr) root = make_node();
        return add_line_rec(root, i, xmin, xmax);
    }

    np add_segment(np root, T xl, T xr, L f) {
        const i32 i = static_cast<i32>(d.size());
        d.push_back(f);

        if (root == nullptr) root = make_node();
        return add_segment_rec(root, xl, xr, i, xmin, xmax);
    }

    std::pair<T, i32> query(np root, T x) {
        assert(xmin <= x && x <= xmax);

        if (root == nullptr) return MIN ? std::pair<T, i32>{inf<T>, -1} : std::pair<T, i32>{-inf<T>, -1};
        return query_rec(root, x, xmin, xmax);
    }

private:
    np copy_node(np c) {
        if constexpr (!PERSIST) return c;
        if (c == nullptr) return c;

        assert(pid < n);
        return &(pool[pid++] = node(*c));
    }

    inline T eval(i32 i, T x) const {
        if (i == -1) return MIN ? inf<T> : -inf<T>;
        return eval(d[i], x);
    }

    static inline T eval(const L &f, T x) {
        return f(x);
    }

    np add_segment_rec(np c, T xl, T xr, i32 i, T l, T r) {
        xl = std::max(xl, l);
        xr = std::min(xr, r);

        if (xl >= xr) return c;
        if (l < xl || xr < r) {
            c = copy_node(c);
            T m = (l + r) / 2;
            if (!c->l) c->l = make_node();
            if (!c->r) c->r = make_node();
            c->l = add_segment_rec(c->l, xl, xr, i, l, m);
            c->r = add_segment_rec(c->r, xl, xr, i, m, r);
            return c;
        }

        return add_line_rec(c, i, l, r);
    }

    np add_line_rec(np c, i32 i, T l, T r) {
        i32 j = c->i;
        T fl = eval(i, l), fr = eval(i, r - 1);
        T gl = eval(j, l), gr = eval(j, r - 1);
        bool bl = (MIN ? fl < gl : fl > gl);
        bool br = (MIN ? fr < gr : fr > gr);
        if (bl && br) {
            c = copy_node(c);
            c->i = i;
            return c;
        }
        if (!bl && !br) return c;

        c = copy_node(c);
        T m = (l + r) / 2;
        auto fm = eval(i, m), gm = eval(j, m);
        bool bm = (MIN ? fm < gm : fm > gm);
        if (bm) {
            c->i = i;
            if (bl) {
                if (!c->r) c->r = make_node();
                c->r = add_line_rec(c->r, j, m, r);
            } else {
                if (!c->l) c->l = make_node();
                c->l = add_line_rec(c->l, j, l, m);
            }
        } else {
            if (!bl) {
                if (!c->r) c->r = make_node();
                c->r = add_line_rec(c->r, i, m, r);
            } else {
                if (!c->l) c->l = make_node();
                c->l = add_line_rec(c->l, i, l, m);
            }
        }

        return c;
    }

    std::pair<T, i32> query_rec(np c, T x, T l, T r) {
        i32 i = c->i;
        std::pair<T, i32> res = {eval(i, x), i};

        const T m = (l + r) / 2;
        if (x < m && c->l) {
            const auto res1 = query_rec(c->l, x, l, m);
            res = (MIN ? std::min(res, res1) : std::max(res, res1));
        }

        if (x >= m && c->r) {
            const auto res1 = query_rec(c->r, x, m, r);
            res = (MIN ? std::min(res, res1) : std::max(res, res1));
        }

        return res;
    }
};

#endif // LIB_SPARSE_LICHAO_TREE_HPP
