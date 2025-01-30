#ifndef LIB_BASIC_LINK_CUT_TREE_HPP
#define LIB_BASIC_LINK_CUT_TREE_HPP 1

#include <algorithm>
#include <cassert>

#include <lib/prelude.hpp>

template <typename T>
struct basic_link_cut_tree {
    using X = T;

    struct node {
        node *l, *r, *p;
        X val;
        bool rev;
        i32 sz;

        explicit node(const X &x)
            : l{nullptr}, r{nullptr}, p{nullptr}, val{x}, rev{false}, sz{1} {}

        node()
            : node(T()) {}

        bool is_root() const {
            return !p || (p->l != this && p->r != this);
        }
    };

    i32 size(node *t) const {
        return t != nullptr ? t->sz : 0;
    }

    i32 n, pid;
    node *pool;

    basic_link_cut_tree()
        : pool{nullptr} {}

    explicit basic_link_cut_tree(i32 m)
        : basic_link_cut_tree() {
        build(m);
    }

    ~basic_link_cut_tree() {
        reset();
    }

    void build(i32 m) {
        reset();

        n = m;
        pool = new node[n];
    }

    void reset() {
        pid = 0;
        delete[] pool;
    }

    node *make_node() const {
        return nullptr;
    }

    node *make_node(const X &x) {
        assert(pid < n);

        return &(pool[pid++] = node(x));
    }

private:
    static void update(node *t) {
        if (t == nullptr) return;

        t->sz = 1;
        if (t->l != nullptr) t->sz += t->l->sz;
        if (t->r != nullptr) t->sz += t->r->sz;
    }

    static void toggle(node *t) {
        std::swap(t->l, t->r);
        t->rev ^= true;
    }

    static void push(node *t) {
        if (t == nullptr) return;

        if (t->rev) {
            if (t->l != nullptr) toggle(t->l);
            if (t->r != nullptr) toggle(t->r);
            t->rev = false;
        }
    }

    static void rotate_right(node *t) {
        node *x = t->p;
        node *y = x->p;

        if ((x->l = t->r)) t->r->p = x;
        t->r = x;
        x->p = t;

        update(x);
        update(t);

        if ((t->p = y)) {
            if (y->l == x) y->l = t;
            if (y->r == x) y->r = t;
            update(y);
        }
    }

    static void rotate_left(node *t) {
        node *x = t->p;
        node *y = x->p;

        if ((x->r = t->l)) t->l->p = x;
        t->l = x;
        x->p = t;

        update(x);
        update(t);

        if ((t->p = y)) {
            if (y->l == x) y->l = t;
            if (y->r == x) y->r = t;
            update(y);
        }
    }

    static void splay(node *t) {
        push(t);

        while (!t->is_root()) {
            node *q = t->p;
            if (q->is_root()) {
                push(q);
                push(t);

                if (q->l == t)
                    rotate_right(t);
                else
                    rotate_left(t);
            } else {
                node *r = q->p;

                push(r);
                push(q);
                push(t);

                if (r->l == q) {
                    if (q->l == t)
                        rotate_right(q);
                    else
                        rotate_left(t);

                    rotate_right(t);
                } else {
                    if (q->r == t)
                        rotate_left(q);
                    else
                        rotate_right(t);

                    rotate_left(t);
                }
            }
        }
    }

public:
    static node *expose(node *t) {
        node *rp = nullptr;
        for (node *cur = t; cur; cur = cur->p) {
            splay(cur);
            cur->r = rp;
            update(cur);
            rp = cur;
        }

        splay(t);
        return rp;
    }

    static void evert(node *t) {
        expose(t);
        toggle(t);
        push(t);
    }

    static void link(node *chi, node *par) {
        evert(chi);
        expose(par);
        chi->p = par;
        par->r = chi;
        update(par);
    }

    static void cut(node *chi) {
        expose(chi);
        node *par = chi->l;
        chi->l = nullptr;
        update(chi);
        par->p = nullptr;
    }

    static void cut(node *u, node *v) {
        evert(u);
        cut(v);
    }

    static node *lca(node *u, node *v) {
        expose(u);
        return expose(v);
    }

    static node *jump(node *t, i32 k) {
        expose(t);

        while (t) {
            push(t);

            if (t->r && t->r->sz > k) {
                t = t->r;
            } else {
                if (t->r) k -= t->r->sz;
                if (k == 0) return t;

                --k;
                t = t->l;
            }
        }

        return nullptr;
    }

    static bool is_connected(node *u, node *v) {
        expose(u);
        expose(v);

        return u == v || u->p;
    }
};

#endif // LIB_BASIC_LINK_CUT_TREE_HPP
