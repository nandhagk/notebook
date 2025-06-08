#ifndef LIB_AUGMENTED_LINK_CUT_TREE_BASE_HPP
#define LIB_AUGMENTED_LINK_CUT_TREE_BASE_HPP 1

#include <cassert>
#include <vector>

#include <lib/algebraic_traits.hpp>
#include <lib/prelude.hpp>

template <typename Node>
struct augmented_link_cut_tree_base {
    using MX = typename Node::MX;
    using MA = typename Node::MA;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    using np = Node *;

    i32 n, pid;
    np pool;
    std::vector<np> free;

    augmented_link_cut_tree_base()
        : pool{nullptr} {}

    explicit augmented_link_cut_tree_base(i32 m)
        : augmented_link_cut_tree_base() {
        build(m);
    }

    ~augmented_link_cut_tree_base() {
        reset();
    }

    void build(i32 m) {
        reset();

        n = m;
        pool = new Node[2 * n + 5];

        free.reserve(n + 5);
        for (i32 i = n; i < 2 * n + 5; ++i) free.push_back(&pool[i]);
    }

    void reset() {
        pid = 0;
        free.clear();
        delete[] pool;
    }

    np make_node() {
        return nullptr;
    }

    np make_node(const X &x) {
        assert(pid < n);

        return &(pool[pid++] = Node(x));
    }

private:
    static void rotate_right_heavy(np t) {
        np x = t->p;
        np y = x->p;

        if ((x->hl = t->hr)) t->hr->p = x;
        t->hr = x;
        x->p = t;

        x->update();
        t->update();

        if ((t->p = y)) {
            if (y->hl == x) y->hl = t;
            if (y->hr == x) y->hr = t;
            if (y->ll == x) y->ll = t;
            if (y->lr == x) y->lr = t;
            y->update();
        }
    }

    static void rotate_left_heavy(np t) {
        np x = t->p;
        np y = x->p;

        if ((x->hr = t->hl)) t->hl->p = x;
        t->hl = x;
        x->p = t;

        x->update();
        t->update();

        if ((t->p = y)) {
            if (y->hl == x) y->hl = t;
            if (y->hr == x) y->hr = t;
            if (y->ll == x) y->ll = t;
            if (y->lr == x) y->lr = t;
            y->update();
        }
    }

    static void splay_heavy(np t) {
        t->push();

        while (!t->is_root_heavy()) {
            np q = t->p;
            if (q->is_root_heavy()) {
                q->push();
                t->push();

                if (q->hl == t)
                    rotate_right_heavy(t);
                else
                    rotate_left_heavy(t);
            } else {
                np r = q->p;

                r->push();
                q->push();
                t->push();

                if (r->hl == q) {
                    if (q->hl == t)
                        rotate_right_heavy(q);
                    else
                        rotate_left_heavy(t);

                    rotate_right_heavy(t);
                } else {
                    if (q->hr == t)
                        rotate_left_heavy(q);
                    else
                        rotate_right_heavy(t);

                    rotate_left_heavy(t);
                }
            }
        }
    }

    static void rotate_right_light(np t) {
        np x = t->p;
        np y = x->p;

        if ((x->ll = t->lr)) t->lr->p = x;
        t->lr = x;
        x->p = t;

        x->update();
        t->update();

        if ((t->p = y)) {
            if (y->ll == x) y->ll = t;
            if (y->lr == x) y->lr = t;
            y->update();
        }
    }

    static void rotate_left_light(np t) {
        np x = t->p;
        np y = x->p;

        if ((x->lr = t->ll)) t->ll->p = x;
        t->ll = x;
        x->p = t;

        x->update();
        t->update();

        if ((t->p = y)) {
            if (y->ll == x) y->ll = t;
            if (y->lr == x) y->lr = t;
            y->update();
        }
    }

    static void splay_light(np t) {
        t->push();

        while (!t->is_root_light()) {
            np q = t->p;
            if (q->is_root_light()) {
                q->push();
                t->push();

                if (q->ll == t)
                    rotate_right_light(t);
                else
                    rotate_left_light(t);
            } else {
                np r = q->p;

                r->push();
                q->push();
                t->push();

                if (r->ll == q) {
                    if (q->ll == t)
                        rotate_right_light(q);
                    else
                        rotate_left_light(t);

                    rotate_right_light(t);
                } else {
                    if (q->lr == t)
                        rotate_left_light(q);
                    else
                        rotate_right_light(t);

                    rotate_left_light(t);
                }
            }
        }
    }

    static void push_rec(np t) {
        if (t->fake) push_rec(t->p);
        t->push();
    }

    void add(np u, np v) {
        if (v == nullptr) return;

        if (u->ll == nullptr) {
            u->ll = v;
            v->p = u;

            u->update();
            return;
        }

        if (u->lr == nullptr) {
            u->lr = v;
            v->p = u;

            u->update();
            return;
        }

        np w = free.back();
        free.pop_back();

        w->ll = u->ll;
        u->ll->p = w;

        w->lr = v;
        v->p = w;

        u->ll = w;
        w->p = u;

        w->update();
        u->update();
    }

    void rem(np u) {
        np v = u->p;
        push_rec(v);

        if (v->fake) {
            np w = v->p;

            if (w->ll == v) {
                if (v->ll == u) {
                    w->ll = v->lr;
                    v->lr->p = w;
                } else {
                    w->ll = v->ll;
                    v->ll->p = w;
                }
            } else if (v->lr == u) {
                w->lr = v->ll;
                v->ll->p = w;
            } else {
                w->lr = v->lr;
                v->lr->p = w;
            }

            w->update();
            if (w->fake) splay_light(w);

            free.push_back(v);
        } else {
            if (v->ll == u)
                v->ll = nullptr;
            else
                v->lr = nullptr;

            v->update();
        }

        u->p = nullptr;
    }

    np par(np u) {
        np v = u->p;
        if (!v->fake) return v;

        splay_light(v);
        return v->p;
    }

public:
    np expose(np u) {
        np v = u;
        splay_heavy(u);

        add(u, u->hr);

        u->hr = nullptr;
        u->update();

        while (u->p) {
            v = par(u);

            splay_heavy(v);
            rem(u);

            add(v, v->hr);

            v->hr = u;
            u->p = v;

            v->update();
            splay_heavy(u);
        }

        return v;
    }

    void evert(np t) {
        expose(t);
        t->toggle();
        t->push();
    }

    void link(np u, np v) {
        evert(u);
        expose(v);
        add(v, u);
    }

    void cut(np u, np v) {
        evert(u);
        expose(v);
        v->hl = u->p = nullptr;
        v->update();
    }

    np lca(np u, np v) {
        expose(u);
        return expose(v);
    }

    void set(np t, const X &x) {
        expose(t);
        t->val = x;
        t->update();
    }

    void multiply(np t, const X &x) {
        expose(t);
        t->val = MX::op(t->val, x);
        t->update();
    }

    X get(np t) {
        expose(t);
        return t->val;
    }

    X prod_path(np u, np v) {
        evert(u);
        expose(v);

        return v->hsum;
    }

    void apply_path(np u, np v, const A &a) {
        evert(u);
        expose(v);

        v->push_heavy(a);
    }

    void apply_subtree(np u, const A &a) {
        expose(u);

        u->val = Node::AM::act(u->val, a, 1);
        if (u->ll != nullptr) u->ll->push_light(true, a);
        if (u->lr != nullptr) u->lr->push_light(true, a);
    }

    void apply_subtree(np u, np p, const A &a) {
        evert(p);
        apply_subtree(u, a);
    }

    X prod_subtree(np u) {
        static_assert(MX::commutative);

        expose(u);

        X res = u->val;
        if (u->ll != nullptr) res = MX::op(u->ll->asum, res);
        if (u->lr != nullptr) res = MX::op(res, u->lr->asum);

        return res;
    }

    X prod_subtree(np u, np p) {
        evert(p);
        return prod_subtree(u);
    }

    bool is_connected(np u, np v) {
        expose(u);
        expose(v);

        return u == v || u->p;
    }
};

#endif // LIB_AUGMENTED_LINK_CUT_TREE_BASE_HPP
