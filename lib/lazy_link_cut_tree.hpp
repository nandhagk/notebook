#include <type_traits>
#ifndef LIB_LAZY_LINK_CUT_TREE_HPP
#define LIB_LAZY_LINK_CUT_TREE_HPP 1

#include <algorithm>
#include <cassert>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename ActedMonoid>
struct lazy_link_cut_tree {
    struct RMX {
        using MX = typename ActedMonoid::MX;
        using X = std::pair<typename MX::ValueT, typename MX::ValueT>;
        using ValueT = X;

        static constexpr X op(const X &x, const X &y) {
            return {MX::op(x.first, y.first), MX::op(y.second, x.second)};
        }

        static constexpr X unit() {
            return {MX::unit(), MX::unit()};
        }

        static constexpr X rev(const X &x) {
            return {x.second, x.first};
        }

        static constexpr bool commutative = MX::commutative;
    };

    struct RAM {
        using MX = RMX;
        using MA = typename ActedMonoid::MA;

        using X = typename MX::ValueT;
        using A = typename MA::ValueT;

        static constexpr X act(const X &x, const A &a, i32 sz) {
            return {ActedMonoid::act(x.first, a, sz), ActedMonoid::act(x.second, a, sz)};
        }
    };

    using AM = std::conditional_t<ActedMonoid::MX::commutative, ActedMonoid, RAM>;

    using MX = typename AM::MX;
    using Y = typename ActedMonoid::MX::ValueT;
    using X = typename MX::ValueT;

    using MA = typename AM::MA;
    using A = typename MA::ValueT;

    struct node {
        node *l, *r, *p;
        X val, sum;
        A lz;
        bool rev;
        u32 sz;

        explicit node(const X &x)
            : l{nullptr}, r{nullptr}, p{nullptr}, val{x}, sum{x}, lz{MA::unit()}, rev{false}, sz{1} {}

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

    lazy_link_cut_tree()
        : pool{nullptr} {}

    explicit lazy_link_cut_tree(i32 m)
        : lazy_link_cut_tree() {
        build(m);
    }

    ~lazy_link_cut_tree() {
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

    node *make_node(const Y &x) {
        assert(pid < n);

        if constexpr (MX::commutative) {
            return &(pool[pid++] = node(x));
        } else {
            return &(pool[pid++] = node({x, x}));
        }
    }

    void update(node *t) {
        if (t == nullptr) return;

        t->sz = 1;
        t->sum = t->val;

        if (t->l != nullptr) {
            t->sz += t->l->sz;
            t->sum = MX::op(t->l->sum, t->sum);
        }

        if (t->r != nullptr) {
            t->sz += t->r->sz;
            t->sum = MX::op(t->sum, t->r->sum);
        }
    }

    void all_apply(node *t, const A &a) {
        t->val = AM::act(t->val, a, 1);
        t->sum = AM::act(t->sum, a, t->sz);
        t->lz = MA::op(t->lz, a);
    }

    void toggle(node *t) {
        std::swap(t->l, t->r);
        if constexpr (!MX::commutative) t->sum = MX::rev(t->sum);

        t->rev ^= true;
    }

    void push(node *t) {
        if (t == nullptr) return;

        if (t->lz != MA::unit()) {
            if (t->l != nullptr) all_apply(t->l, t->lz);
            if (t->r != nullptr) all_apply(t->r, t->lz);
            t->lz = MA::unit();
        }

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

    void set(node *t, const Y &x) {
        expose(t);

        if constexpr (MX::commutative) {
            t->val = x;
        } else {
            t->val = {x, x};
        }

        update(t);
    }

    void multiply(node *t, const Y &x) {
        expose(t);
        
        if constexpr (MX::commutative) {
            t->val = MX::op(t->val, x);
        } else {
            t->val = MX::op(t->val, {x, x});
        }

        update(t);
    }

    Y get(node *t) {
        expose(t);
        
        if constexpr (MX::commutative) {
            return t->val;
        } else {
            return t->val.first;
        }
    }

    Y prod(node *u, node *v) {
        evert(u);
        expose(v);
        
        if constexpr (MX::commutative) {
            return v->sum;
        } else {
            return v->sum.first;
        }
    }

    void apply(node *u, node *v, const A &a) {
        evert(u);
        expose(v);
        all_apply(v, a);
        push(v);
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

#endif // LIB_LAZY_LINK_CUT_TREE_HPP
