#ifndef LIB_AUGMENTED_LINK_CUT_TREE_HPP
#define LIB_AUGMENTED_LINK_CUT_TREE_HPP 1

#include <algorithm>
#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_monoid_t<Monoid> * = nullptr>
struct augmented_link_cut_tree {
    using MX = Monoid;
    using X = typename MX::ValueT;

    struct node {
        node *hl, *hr, *ll, *lr, *p;
        X val, hsum, hmus, lsum, asum;
        bool rev, fake;
        i32 hsz, lsz, asz;

        node()
            : hl{nullptr},
              hr{nullptr},
              ll{nullptr},
              lr{nullptr},
              p{nullptr},
              val{MX::unit()},
              hsum{MX::unit()},
              hmus{MX::unit()},
              lsum{MX::unit()},
              asum{MX::unit()},
              rev{false},
              fake{true},
              hsz{0},
              lsz{0},
              asz{0} {}

        explicit node(const X &x)
            : node() {
            val = hsum = hmus = asum = x;
            fake = false;
            hsz = asz = 1;
        }

        bool is_root_heavy() {
            return !p || (p->hl != this && p->hr != this);
        }

        bool is_root_light() {
            return !p || (p->ll != this && p->lr != this) || !p->fake;
        }
    };

    i32 n, pid;
    node *pool;
    std::vector<node *> free;

    augmented_link_cut_tree()
        : pool{nullptr} {}

    explicit augmented_link_cut_tree(i32 m)
        : augmented_link_cut_tree() {
        build(m);
    }

    ~augmented_link_cut_tree() {
        reset();
    }

    void build(i32 m) {
        reset();

        n = m;
        pool = new node[2 * n + 5];

        free.reserve(n + 5);
        for (i32 i = n; i < 2 * n + 5; ++i) free.push_back(&pool[i]);
    }

    void reset() {
        pid = 0;
        free.clear();
        delete[] pool;
    }

    node *make_node() {
        return nullptr;
    }

    node *make_node(const X &x) {
        assert(pid < n);

        return &(pool[pid++] = node(x));
    }

private:
    static void update(node *t) {
        if (!t->fake) {
            t->hsz = 1;
            t->hmus = t->hsum = t->val;

            if (t->hl != nullptr) {
                t->hsz += t->hl->hsz;
                t->hsum = MX::op(t->hl->hsum, t->hsum);

                if constexpr (!MX::commutative) t->hmus = MX::op(t->hmus, t->hl->hmus);
            }

            if (t->hr != nullptr) {
                t->hsz += t->hr->hsz;
                t->hsum = MX::op(t->hsum, t->hr->hsum);

                if constexpr (!MX::commutative) t->hmus = MX::op(t->hr->hmus, t->hmus);
            }
        }

        t->lsz = 0;
        t->lsum = MX::unit();

        if (t->hl != nullptr) {
            t->lsz += t->hl->lsz;
            t->lsum = MX::op(t->hl->lsum, t->lsum);
        }

        if (t->hr != nullptr) {
            t->lsz += t->hr->lsz;
            t->lsum = MX::op(t->lsum, t->hr->lsum);
        }

        if (t->ll != nullptr) {
            t->lsz += t->ll->asz;
            t->lsum = MX::op(t->ll->asum, t->lsum);
        }

        if (t->lr != nullptr) {
            t->lsz += t->lr->asz;
            t->lsum = MX::op(t->lsum, t->lr->asum);
        }

        t->asz = t->hsz + t->lsz;
        t->asum = MX::op(t->hsum, t->lsum);
    }

    static void toggle(node *t) {
        if (t == nullptr) return;

        std::swap(t->hl, t->hr);
        t->rev ^= true;

        if constexpr (!MX::commutative) std::swap(t->hsum, t->hmus);
    }

    static void push(node *t) {
        if (t == nullptr) return;

        if (t->rev) {
            if (t->hl != nullptr) toggle(t->hl);
            if (t->hr != nullptr) toggle(t->hr);
            t->rev = false;
        }
    }

    static void rotate_right_heavy(node *t) {
        node *x = t->p;
        node *y = x->p;

        if ((x->hl = t->hr)) t->hr->p = x;
        t->hr = x;
        x->p = t;

        update(x);
        update(t);

        if ((t->p = y)) {
            if (y->hl == x) y->hl = t;
            if (y->hr == x) y->hr = t;
            if (y->ll == x) y->ll = t;
            if (y->lr == x) y->lr = t;
            update(y);
        }
    }

    static void rotate_left_heavy(node *t) {
        node *x = t->p;
        node *y = x->p;

        if ((x->hr = t->hl)) t->hl->p = x;
        t->hl = x;
        x->p = t;

        update(x);
        update(t);

        if ((t->p = y)) {
            if (y->hl == x) y->hl = t;
            if (y->hr == x) y->hr = t;
            if (y->ll == x) y->ll = t;
            if (y->lr == x) y->lr = t;
            update(y);
        }
    }

    static void splay_heavy(node *t) {
        push(t);

        while (!t->is_root_heavy()) {
            node *q = t->p;
            if (q->is_root_heavy()) {
                push(q);
                push(t);

                if (q->hl == t)
                    rotate_right_heavy(t);
                else
                    rotate_left_heavy(t);
            } else {
                node *r = q->p;

                push(r);
                push(q);
                push(t);

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

    static void rotate_right_light(node *t) {
        node *x = t->p;
        node *y = x->p;

        if ((x->ll = t->lr)) t->lr->p = x;
        t->lr = x;
        x->p = t;

        update(x);
        update(t);

        if ((t->p = y)) {
            if (y->ll == x) y->ll = t;
            if (y->lr == x) y->lr = t;
            update(y);
        }
    }

    static void rotate_left_light(node *t) {
        node *x = t->p;
        node *y = x->p;

        if ((x->lr = t->ll)) t->ll->p = x;
        t->ll = x;
        x->p = t;

        update(x);
        update(t);

        if ((t->p = y)) {
            if (y->ll == x) y->ll = t;
            if (y->lr == x) y->lr = t;
            update(y);
        }
    }

    static void splay_light(node *t) {
        push(t);

        while (!t->is_root_light()) {
            node *q = t->p;
            if (q->is_root_light()) {
                push(q);
                push(t);

                if (q->ll == t)
                    rotate_right_light(t);
                else
                    rotate_left_light(t);
            } else {
                node *r = q->p;

                push(r);
                push(q);
                push(t);

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

    static void push_rec(node *t) {
        if (t->fake) push_rec(t->p);
        push(t);
    }

    void add(node *u, node *v) {
        if (v == nullptr) return;

        if (u->ll == nullptr) {
            u->ll = v;
            v->p = u;

            update(u);
            return;
        }

        if (u->lr == nullptr) {
            u->lr = v;
            v->p = u;

            update(u);
            return;
        }

        node *w = free.back();
        free.pop_back();

        w->ll = u->ll;
        u->ll->p = w;

        w->lr = v;
        v->p = w;

        u->ll = w;
        w->p = u;

        update(w);
        update(u);
    }

    void rem(node *u) {
        node *v = u->p;
        push_rec(v);

        if (v->fake) {
            node *w = v->p;

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

            update(w);
            if (w->fake) splay_light(w);

            free.push_back(v);
        } else {
            if (v->ll == u)
                v->ll = nullptr;
            else
                v->lr = nullptr;

            update(v);
        }

        u->p = nullptr;
    }

    static node *par(node *u) {
        node *v = u->p;
        if (!v->fake) return v;

        splay_light(v);
        return v->p;
    }

public:
    node *expose(node *u) {
        node *v = u;
        splay_heavy(u);

        add(u, u->hr);

        u->hr = nullptr;
        update(u);

        while (u->p) {
            v = par(u);

            splay_heavy(v);
            rem(u);

            add(v, v->hr);

            v->hr = u;
            u->p = v;

            update(v);
            splay_heavy(u);
        }

        return v;
    }

    void evert(node *t) {
        expose(t);
        toggle(t);
        push(t);
    }

    void link(node *u, node *v) {
        evert(u);
        expose(v);
        add(v, u);
    }

    void cut(node *u, node *v) {
        evert(u);
        expose(v);
        v->hl = u->p = nullptr;
        update(v);
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

        return v->hsum;
    }

    X prod_subtree(node *u) {
        static_assert(MX::commutative);

        expose(u);

        X res = u->val;
        if (u->ll != nullptr) res = MX::op(u->ll->asum, res);
        if (u->lr != nullptr) res = MX::op(res, u->lr->asum);

        return res;
    }

    X prod_subtree(node *u, node *p) {
        evert(p);
        return prod_subtree(u);
    }

    bool is_connected(node *u, node *v) {
        expose(u);
        expose(v);

        return u == v || u->p;
    }
};

#endif // LIB_AUGMENTED_LINK_CUT_TREE_HPP
