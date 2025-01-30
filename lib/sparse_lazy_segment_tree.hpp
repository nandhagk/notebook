#ifndef LIB_SPARSE_LAZY_SEGMENT_TREE_HPP
#define LIB_SPARSE_LAZY_SEGMENT_TREE_HPP 1

#include <algorithm>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename T, typename ActedMonoid, is_integral_t<T> * = nullptr>
struct sparse_lazy_segment_tree {
    using AM = ActedMonoid;

    using MX = typename AM::MX;
    using MA = typename AM::MA;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

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
        A lz;
        node(T a, T b, const X &v)
            : lx(a),
              rx(b),
              Lx(a),
              Rx(b),
              l(nullptr),
              r(nullptr),
              val(v),
              sum(pow_monoid(v, rx - lx)),
              sum_subtree(sum),
              lz(MA::unit()) {}

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

    static void push_down(node *v) {
        if (v->lz != MA::unit()) {
            if (v->l) all_apply(v->l, v->lz);
            if (v->r) all_apply(v->r, v->lz);
            v->lz = MA::unit();
        }
    }

    static void all_apply(node *v, const A &lz) {
        v->lz = MA::op(v->lz, lz);
        v->val = AM::act(v->val, lz, 1);
        v->sum = AM::act(v->sum, lz, v->rx - v->lx);
        v->sum_subtree = AM::act(v->sum_subtree, lz, v->Rx - v->Lx);
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

        push_down(v);
        v->l = insert_leftmost(v->l, u);
        update(v);

        return balance(v);
    }

    static node *insert_rightmost(node *v, node *u) {
        if (!v) return u;

        push_down(v);
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
    sparse_lazy_segment_tree(T minf, T inf)
        : root(new node(minf, inf, MX::unit())) {}

    ~sparse_lazy_segment_tree() {
        clear(root);
    }

    void set(T p, const X &x) {
        const auto dfs = [&](auto &&self, node *v) -> node * {
            if (!v || p < v->Lx || v->Rx <= p) return v;

            push_down(v);
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
        while (v) {
            push_down(v);
            if (p < v->lx)
                v = v->l;
            else if (v->rx <= p)
                v = v->r;
            else
                return v->val;
        }
    }

    void apply(T l, T r, const A &lz) {
        const auto dfs = [&](auto &&self, node *v) -> node * {
            if (!v || r <= v->Lx || v->Rx <= l) return v;
            if (l <= v->Lx && v->Rx <= r) {
                all_apply(v, lz);
                return v;
            }

            push_down(v);
            v->l = self(self, v->l);
            v->r = self(self, v->r);

            T L = std::max(l, v->lx), R = std::min(r, v->rx);
            if (L < R) {
                if (L != v->lx || R != v->rx) {
                    auto [a, c] = split(v, l, r);
                    if (a) v->l = insert_rightmost(v->l, a);
                    if (c) v->r = insert_leftmost(v->r, c);
                }

                v->val = AM::act(v->val, lz, 1);
                v->sum = AM::act(v->sum, lz, v->rx - v->lx);
            }

            update(v);
            return balance(v);
        };

        root = dfs(dfs, root);
    }

    X all_prod() {
        return (root ? root->sum_subtree : MX::unit());
    }

    X prod(T l, T r) {
        const auto dfs = [&](auto &&self, node *v) -> X {
            if (!v || r <= v->Lx || v->Rx <= l) return MX::unit();
            if (l <= v->Lx && v->Rx <= r) return v->sum_subtree;

            push_down(v);

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

    template <typename G>
    T max_right(G g, T l) {
        assert(g(MX::unit()));

        X lsum = MX::unit();
        const auto dfs = [&](auto &&self, node *v) -> void {
            if (!v) return;
            if (v->Lx == l && g(MX::op(lsum, v->sum_subtree))) {
                l = v->Rx;
                lsum = MX::op(lsum, v->sum_subtree);
                return;
            }

            push_down(v);
            if (v->rx <= l) {
                self(self, v->r);
                return;
            }

            if (l < v->lx) self(self, v->l);
            if (l < v->lx) return;

            X x = (v->lx == l ? v->sum : pow_monoid(v->val, v->rx - l));
            if (g(MX::op(lsum, x))) {
                l = v->rx;
                lsum = MX::op(lsum, x);
            } else {
                T len = 1;
                std::vector<X> p2{v->val};

                while (len < v->rx - l) {
                    X s = p2.back();
                    p2.push_back(MX::op(s, s));
                    len *= 2;
                }

                for (i32 i = static_cast<i32>(p2.size()) - 1; i >= 0; i--) {
                    len = T(1) << i;
                    if ((v->rx - l) >= len && g(MX::op(lsum, p2[i]))) {
                        l += len;
                        lsum = MX::op(lsum, p2[i]);
                    }
                }
                return;
            }

            self(self, v->r);
        };

        return dfs(dfs, root);
    }

    template <typename G>
    T min_left(G g, T r) {
        assert(g(MX::unit()));

        X rsum = MX::unit();
        const auto dfs = [&](auto &&self, node *v) -> void {
            if (!v) return;
            if (v->Rx == r && g(MX::op(v->sum_subtree, rsum))) {
                r = v->Lx;
                rsum = MX::op(v->sum_subtree, rsum);
                return;
            }

            push_down(v);
            if (v->lx >= r) {
                self(self, v->l);
                return;
            }

            if (r > v->rx) self(self, v->r);
            if (r > v->rx) return;

            X x = (v->rx == r ? v->sum : pow_monoid(v->val, r - v->lx));
            if (g(MX::op(x, rsum))) {
                r = v->lx;
                rsum = MX::op(x, rsum);
            } else {
                T len = 1;
                std::vector<X> p2{v->val};
                while (len < r - v->lx) {
                    X s = p2.back();
                    p2.push_back(MX::op(s, s));
                    len *= 2;
                }

                for (i32 i = static_cast<i32>(p2.size()) - 1; i >= 0; i--) {
                    len = T(1) << i;
                    if ((r - v->lx) >= len && MX::op(p2[i], rsum)) {
                        r -= len;
                        rsum = MX::op(p2[i], rsum);
                    }
                }
                return;
            }

            self(self, v->l);
        };

        return dfs(dfs, root);
    }
};

#endif // LIB_SPARSE_LAZY_SEGMENT_TREE_HPP
