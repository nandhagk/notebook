#ifndef LIB_LINK_CUT_TREE_BASE_HPP
#define LIB_LINK_CUT_TREE_BASE_HPP 1

#include <cassert>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename Node>
struct link_cut_tree_base {
    using MX = typename Node::MX;
    using MA = typename Node::MA;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    using np = Node *;

    i32 n, pid;
    np pool;

    link_cut_tree_base()
        : pool{nullptr} {}

    explicit link_cut_tree_base(i32 m)
        : link_cut_tree_base() {
        build(m);
    }

    ~link_cut_tree_base() {
        reset();
    }

    void build(i32 m) {
        reset();

        n = m;
        pool = new Node[n];
    }

    void reset() {
        pid = 0;
        delete[] pool;
    }

    np make_node() const {
        return nullptr;
    }

    np make_node(const X &x) {
        assert(pid < n);

        return &(pool[pid++] = Node(x));
    }

private:
    static void rotate_right(np t) {
        np x = t->p;
        np y = x->p;

        if ((x->l = t->r)) t->r->p = x;
        t->r = x;
        x->p = t;

        x->update();
        t->update();

        if ((t->p = y)) {
            if (y->l == x) y->l = t;
            if (y->r == x) y->r = t;
            y->update();
        }
    }

    static void rotate_left(np t) {
        np x = t->p;
        np y = x->p;

        if ((x->r = t->l)) t->l->p = x;
        t->l = x;
        x->p = t;

        x->update();
        t->update();

        if ((t->p = y)) {
            if (y->l == x) y->l = t;
            if (y->r == x) y->r = t;
            y->update();
        }
    }

    static void splay(np t) {
        t->push();

        while (!t->is_root()) {
            np q = t->p;
            if (q->is_root()) {
                q->push();
                t->push();

                if (q->l == t)
                    rotate_right(t);
                else
                    rotate_left(t);
            } else {
                np r = q->p;

                r->push();
                q->push();
                t->push();

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
    static np expose(np t) {
        np rp = nullptr;
        for (np cur = t; cur; cur = cur->p) {
            splay(cur);
            cur->r = rp;
            cur->update();
            rp = cur;
        }

        splay(t);
        return rp;
    }

    static void evert(np t) {
        expose(t);
        t->toggle();
        t->push();
    }

    static void link(np chi, np par) {
        evert(chi);
        expose(par);
        chi->p = par;
        par->r = chi;
        par->update();
    }

    static void cut(np chi) {
        expose(chi);
        np par = chi->l;
        chi->l = nullptr;
        chi->update();
        par->p = nullptr;
    }

    static void cut(np u, np v) {
        evert(u);
        cut(v);
    }

    static np lca(np u, np v) {
        expose(u);
        return expose(v);
    }

    static void set(np t, const X &x) {
        expose(t);
        t->val = x;
        t->update();
    }

    static void multiply(np t, const X &x) {
        expose(t);
        t->val = MX::op(t->val, x);
        t->update();
    }

    static X get(np t) {
        expose(t);
        return t->val;
    }

    static X prod_path(np u, np v) {
        evert(u);
        expose(v);
        return v->sum;
    }

    static void apply_path(np u, np v, const A &a) {
        evert(u);
        expose(v);

        v->all_apply(a);
        v->push();
    }

    static np jump(np t, i32 k) {
        expose(t);
        while (t) {
            t->push();
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

    static bool is_connected(np u, np v) {
        expose(u);
        expose(v);

        return u == v || u->p;
    }
};

#endif // LIB_LINK_CUT_TREE_BASE_HPP
