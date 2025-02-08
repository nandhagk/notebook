#ifndef LIB_SPLAY_TREE_BASE_HPP
#define LIB_SPLAY_TREE_BASE_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

// Can be O(n) with cut and paste use treap instead
template <typename Node>
struct splay_tree_base {
    using np = Node *;

    using MX = typename Node::MX;
    using MA = typename Node::MA;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    static i32 size(np t) {
        return t != nullptr ? t->sz : 0;
    }

    i32 n, pid;
    np pool;

    splay_tree_base()
        : pool{nullptr} {}

    explicit splay_tree_base(i32 m)
        : splay_tree_base() {
        build(m);
    }

    ~splay_tree_base() {
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

    np make_node() {
        return nullptr;
    }

    np make_node(const X &x) {
        assert(pid < n);

        return &(pool[pid++] = Node(x));
    }

    np make_nodes(i32 p) {
        return make_nodes(p, [](i32) -> X { return MX::unit(); });
    }

    np make_nodes(const std::vector<X> &v) {
        return make_nodes(static_cast<i32>(v.size()), [&](i32 i) -> X { return v[i]; });
    }

    template <typename F>
    np make_nodes(i32 p, F f) {
        return assign(0, p, f);
    }

    template <typename F>
    np assign(i32 l, i32 r, F f) {
        if (l == r) return make_node();
        if (l + 1 == r) return make_node(f(l));

        const i32 m = (l + r) / 2;

        np t = make_node(f(m));
        np a = assign(l, m, f);
        np b = assign(m + 1, r, f);

        t->l = a;
        t->r = b;

        if (a) a->p = t;
        if (b) b->p = t;

        t->update();
        return t;
    }

    static np merge(np l, np r) {
        if (!l) return r;
        if (!r) return l;
        assert((!l->p) && (!r->p));

        splay_kth(r, 0);
        r->l = l;
        l->p = r;
        r->update();

        return r;
    }

    static np merge3(np a, np b, np c) {
        return merge(merge(a, b), c);
    }

    static np merge4(np a, np b, np c, np d) {
        return merge(merge(merge(a, b), c), d);
    }

    static std::pair<np, np> split(np root, i32 k) {
        assert(!root || !root->p);

        if (k == 0) return {nullptr, root};
        if (k == size(root)) return {root, nullptr};

        splay_kth(root, k - 1);

        np r = root->r;
        root->r = nullptr;
        r->p = nullptr;

        root->update();
        return {root, r};
    }

    static std::tuple<np, np, np> split3(np root, i32 l, i32 r) {
        np nm, nr;
        std::tie(root, nr) = split(root, r);
        std::tie(root, nm) = split(root, l);
        return {root, nm, nr};
    }

    void insert(np &root, i32 k, const X &x) {
        insert(root, k, make_node(x));
    }

    void erase(np &root, i32 k) {
        const auto [a, b, c] = split3(root, k, k + 1);
        root = merge(a, c);
    }

    static void insert(np &root, i32 k, np t) {
        const auto [a, b] = split(root, k);
        root = merge3(a, t, b);
    }

    static std::vector<X> get_all(const np &root) {
        std::vector<X> res;
        res.reserve(size(root));

        const auto dfs = [&](auto &&self, np t) -> void {
            if (!t) return;

            t->push();
            self(self, t->l);
            res.push_back(t->val);
            self(self, t->r);
        };

        dfs(dfs, root);
        return res;
    }

    static X get(np &root, i32 k) {
        assert(root == nullptr || !root->p);

        splay_kth(root, k);
        return root->val;
    }

    static void set(np &root, i32 k, const X &x) {
        assert(root != nullptr && !root->p);

        splay_kth(root, k);
        root->val = x;
        root->update();
    }

    static void multiply(np &root, i32 k, const X &x) {
        assert(root != nullptr && !root->p);

        splay_kth(root, k);
        root->val = MX::op(root->val, x);
        root->update();
    }

    static X prod(np &root, i32 l, i32 r) {
        assert(root == nullptr || !root->p);
        if (l == r) return MX::unit();

        assert(0 <= l && l < r && r <= size(root));

        goto_between(root, l, r);
        X res = root->sum;
        splay(root, true);
        return res;
    }

    static X prod_all(np &root) {
        assert(root == nullptr || !root->p);
        return (root ? root->sum : MX::unit());
    }

    static void apply(np &root, i32 l, i32 r, const A &a) {
        if (l == r) return;

        assert(0 <= l && l < r && r <= size(root));
        goto_between(root, l, r);

        root->all_apply(a);
        splay(root, true);
    }

    static void apply(np &root, const A &a) {
        if (!root) return;
        root->all_apply(a);
    }

    static void reverse(np &root, i32 l, i32 r) {
        assert(root == nullptr || !root->p);

        if (l == r) return;
        assert(0 <= l && l < r && r <= size(root));

        goto_between(root, l, r);
        root->toggle();
        splay(root, true);
    }

    static void reverse(np root) {
        if (!root) return;
        root->toggle();
    }

private:
    static void rotate(np n) {
        np pp, p, c;
        p = n->p;
        pp = p->p;

        if (p->l == n) {
            c = n->r;
            n->r = p;
            p->l = c;
        } else {
            c = n->l;
            n->l = p;
            p->r = c;
        }

        if (pp && pp->l == p) pp->l = n;
        if (pp && pp->r == p) pp->r = n;
        n->p = pp;
        p->p = n;
        if (c) c->p = p;
    }

    static void prop_from_root(np c) {
        if (!c->p) {
            c->push();
            return;
        }

        prop_from_root(c->p);
        c->push();
    }

    static void splay(np me, bool prop_from_root_done) {
        if (!prop_from_root_done) prop_from_root(me);

        me->push();
        while (me->p) {
            np p = me->p;
            np pp = p->p;
            if (!pp) {
                rotate(me);
                p->update();
                break;
            }

            bool same = (p->l == me && pp->l == p) || (p->r == me && pp->r == p);
            if (same) {
                rotate(p);
                rotate(me);
            } else {
                rotate(me);
                rotate(me);
            }

            pp->update();
            p->update();
        }

        me->update();
    }

    static void splay_kth(np &root, i32 k) {
        assert(0 <= k && k < size(root));

        for (;;) {
            root->push();
            i32 sl = size(root->l);
            if (k == sl) break;
            if (k < sl) {
                root = root->l;
            } else {
                k -= sl + 1;
                root = root->r;
            }
        }

        splay(root, true);
    }

    static void goto_between(np &root, i32 l, i32 r) {
        if (l == 0 && r == size(root)) return;

        if (l == 0) {
            splay_kth(root, r);
            root = root->l;
            return;
        }

        if (r == size(root)) {
            splay_kth(root, l - 1);
            root = root->r;
            return;
        }

        splay_kth(root, r);

        np rp = root;
        root = rp->l;
        root->p = nullptr;

        splay_kth(root, l - 1);

        root->p = rp;
        rp->l = root;
        rp->update();

        root = root->r;
    }
};

#endif // LIB_SPLAY_TREE_BASE_HPP
