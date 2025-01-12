#ifndef LIB_LINK_CUT_TREE_HPP
#define LIB_LINK_CUT_TREE_HPP 1

#include <algorithm>
#include <cassert>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_monoid_t<Monoid> * = nullptr>
struct link_cut_tree {
    using MX = Monoid;
    using X = typename MX::ValueT;

    struct node {
        node *l, *r, *p;
        X val, sum, mus;
        bool rev;
        u32 sz;

        explicit node(const X &x)
            : l{nullptr}, r{nullptr}, p{nullptr}, val{x}, sum{x}, mus{x}, rev{false}, sz{1} {}

        node()
            : node(MX::unit()) {}

        bool is_root() const {
            return !p || (p->l != this && p->r != this);
        }
    };

    i32 size(node *t) const {
        return t != nullptr ? static_cast<i32>(t->sz) : 0;
    }

    i32 n, pid;
    node *pool;

    link_cut_tree()
        : pool{nullptr} {}

    explicit link_cut_tree(i32 m)
        : link_cut_tree() {
        build(m);
    }

    ~link_cut_tree() {
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

    node *make_node() {
        return nullptr;
    }

    node *make_node(const X &x) {
        assert(pid < n);

        return &(pool[pid++] = node(x));
    }

    void update(node *t) {
        if (t == nullptr) return;

        t->sz = 1;
        t->mus = t->sum = t->val;

        if (t->l != nullptr) {
            t->sz += t->l->sz;
            t->sum = MX::op(t->l->sum, t->sum);
            t->mus = MX::op(t->mus, t->l->mus);
        }

        if (t->r != nullptr) {
            t->sz += t->r->sz;
            t->sum = MX::op(t->sum, t->r->sum);
            t->mus = MX::op(t->r->mus, t->mus);
        }
    }

    void toggle(node *t) {
        std::swap(t->l, t->r);
        std::swap(t->sum, t->mus);
        t->rev ^= true;
    }

    void push(node *t) {
        if (t == nullptr) return;

        if (t->rev) {
            if (t->l != nullptr) toggle(t->l);
            if (t->r != nullptr) toggle(t->r);
            t->rev = false;
        }
    }

    void rotate_right(node *t) {
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

    void rotate_left(node *t) {
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

    void splay(node *t) {
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

    void evert(node *t) {
        expose(t);
        toggle(t);
        push(t);
    }

    node *expose(node *t) {
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

    void link(node *chi, node *par) {
        evert(chi);
        expose(par);
        chi->p = par;
        par->r = chi;
        update(par);
    }

    void cut(node *chi) {
        expose(chi);
        node *par = chi->l;
        chi->l = nullptr;
        update(chi);
        par->p = nullptr;
    }

    void cut(node *u, node *v) {
        evert(u);
        cut(v);
    }

    node *lca(node *u, node *v) {
        expose(u);
        return expose(v);
    }

    void set(node *t, const X &x) {
        expose(t);
        t->val = x;
        update(t);
    }

    void multiply(node *t, const X &x) {
        expose(t);
        t->val = MX::op(t->val, x);
        update(t);
    }

    X get(node *t) {
        expose(t);
        return t->val;
    }

    X prod_path(node *u, node *v) {
        evert(u);
        expose(v);
        return v->sum;
    }

    node *jump(node *t, i32 k) {
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

    bool is_connected(node *u, node *v) {
        expose(u);
        expose(v);
        return u == v || u->p;
    }
};

#endif // LIB_LINK_CUT_TREE_HPP
