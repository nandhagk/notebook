#ifndef LIB_SPARSE_SEGMENT_TREE_HPP
#define LIB_SPARSE_SEGMENT_TREE_HPP 1

#include <algorithm>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename T, typename Monoid, is_integral_t<T> * = nullptr, is_monoid_t<Monoid> * = nullptr>
struct sparse_segment_tree {
    using MX = Monoid;
    using X = typename MX::ValueT;

private:
    static constexpr X pow_monoid(const X &a, T k) {
        if constexpr (has_pow_v<MX>) return MX::pow(a, static_cast<i64>(k));

        X b = MX::unit();
        for (X c = a; k; c = MX::op(c, c), k >>= 1)
            if (k & 1) b = MX::op(b, c);

        return b;
    }

    struct node {
        i32 h;
        T lx, rx, Lx, Rx;
        node *l, *r;
        X val, sum, sum_subtree;
        node(T a, T b, const X &v)
            : lx(a),
              rx(b),
              Lx(a),
              Rx(b),
              l(nullptr),
              r(nullptr),
              val(v),
              sum(pow_monoid(v, rx - lx)),
              sum_subtree(sum) {}

        i32 factor() const {
            return (l ? l->h : 0) - (r ? r->h : 0);
        }
    };

    static void update(node *v) {
        v->h = 1;
        v->Lx = v->lx;
        v->Rx = v->rx;
        v->sum_subtree = v->sum;
        if (v->l) {
            v->h = v->l->h + 1;
            v->Lx = v->l->Lx;
            v->sum_subtree = MX::op(v->l->sum_subtree, v->sum_subtree);
        }
        if (v->r) {
            v->h = std::max(v->h, v->r->h + 1);
            v->Rx = v->r->Rx;
            v->sum_subtree = MX::op(v->sum_subtree, v->r->sum_subtree);
        }
    }

    static node *rotate_right(node *v) {
        node *l = v->l;
        v->l = l->r;
        l->r = v;
        update(v);
        update(l);
        return l;
    }

    static node *rotate_left(node *v) {
        node *r = v->r;
        v->r = r->l;
        r->l = v;
        update(v);
        update(r);
        return r;
    }

    static node *balance(node *v) {
        const i32 bf = v->factor();

        if (bf == 2) {
            if (v->l->factor() == -1) {
                v->l = rotate_left(v->l);
                update(v);
            }

            return rotate_right(v);
        } else if (bf == -2) {
            if (v->r->factor() == 1) {
                v->r = rotate_right(v->r);
                update(v);
            }

            return rotate_left(v);
        }

        return v;
    }

    static node *insert_leftmost(node *v, node *u) {
        if (!v) return u;

        v->l = insert_leftmost(v->l, u);
        update(v);

        return balance(v);
    }

    static node *insert_rightmost(node *v, node *u) {
        if (!v) return u;

        v->r = insert_rightmost(v->r, u);
        update(v);

        return balance(v);
    }

    static std::pair<node *, node *> split(node *v, T l, T r) {
        l = std::max(l, v->lx);
        r = std::min(r, v->rx);
        assert(l < r);

        node *a = nullptr, *c = nullptr;
        if (v->lx < l) a = new node(v->lx, l, v->val);
        if (r < v->rx) c = new node(r, v->rx, v->val);

        v->lx = l, v->rx = r;
        v->sum = pow_monoid(v->val, r - l);

        return {a, c};
    }

    node *root;

    void clear(node *v) {
        if (v == nullptr) return;

        clear(v->l);
        clear(v->r);

        delete v;
    }

public:
    sparse_segment_tree(T minf, T inf)
        : root(new node(minf, inf, MX::unit())) {}

    ~sparse_segment_tree() {
        clear(root);
    }

    void set(T p, const X &x) {
        const auto dfs = [&](auto &&self, node *v) -> node * {
            if (!v || p < v->Lx || v->Rx <= p) return v;

            if (p < v->lx) {
                v->l = self(self, v->l);
            } else if (v->rx <= p) {
                v->r = self(self, v->r);
            } else {
                auto [a, c] = split(v, p, p + 1);
                if (a) v->l = insert_rightmost(v->l, a);
                if (c) v->r = insert_leftmost(v->r, c);
                v->val = v->sum = x;
            }

            update(v);
            return balance(v);
        };

        root = dfs(dfs, root);
    }

    X get(T p) {
        node *v = root;
        while (v)
            if (p < v->lx)
                v = v->l;
            else if (v->rx <= p)
                v = v->r;
            else
                return v->val;
    }

    X prod_all() {
        return (root ? root->sum_subtree : MX::unit());
    }

    X prod(T l, T r) {
        const auto dfs = [&](auto &&self, node *v) -> X {
            if (!v || r <= v->Lx || v->Rx <= l) return MX::unit();
            if (l <= v->Lx && v->Rx <= r) return v->sum_subtree;

            X mid = MX::unit();
            T L = std::max(l, v->lx), R = std::min(r, v->rx);
            if (L < R) {
                if (l <= v->lx && v->rx <= r)
                    mid = v->sum;
                else
                    mid = pow_monoid(v->val, R - L);
            }

            return MX::op(self(self, v->l), MX::op(mid, self(self, v->r)));
        };

        return dfs(dfs, root);
    }
};

#endif // LIB_SPARSE_SEGMENT_TREE_HPP
