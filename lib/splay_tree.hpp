#ifndef LIB_SPLAY_TREE_HPP
#define LIB_SPLAY_TREE_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>

template <typename Monoid>
struct splay_tree {
    using MX = Monoid;
    using X = typename MX::ValueT;

    struct node {
        node *l, *r;
        X val, sum;
        bool rev;
        u32 sz;

        explicit node(X x) : l{nullptr}, r{nullptr}, val{x}, sum{x}, rev{false}, sz{1} {}

        node() : node(MX::unit()) {}
    };

    i32 size(node *t) const { return t != nullptr ? static_cast<i32>(t->sz) : 0; }

    i32 n, pid;
    node *pool;

    splay_tree() : pool{nullptr} {}

    explicit splay_tree(i32 m) : splay_tree() { build(m); }

    ~splay_tree() { reset(); }

    void build(i32 m) {
        reset();

        n = m;
        pool = new node[n];
    }

    void reset() {
        pid = 0;
        delete[] pool;
    }

    node *make_node() { return nullptr; }

    node *make_node(X x) {
        assert(pid < n);

        return &(pool[pid++] = node(x));
    }

    node *make_nodes(i32 p) {
        return make_nodes(p, [](i32) -> X { return MX::unit(); });
    }

    node *make_nodes(const std::vector<X> &v) {
        return make_nodes(static_cast<i32>(v.size()), [&](i32 i) -> X { return v[i]; });
    }

    template <typename F>
    node *make_nodes(i32 p, F f) {
        return assign(0, p, f);
    }

    template <typename F>
    node *assign(i32 l, i32 r, F f) {
        if (l == r) return make_node();
        if (l + 1 == r) return make_node(f(l));

        const i32 m = (l + r) / 2;

        node *t = make_node(f(m));
        t->l = assign(l, m, f);
        t->r = assign(m + 1, r, f);

        update(t);
        return t;
    }

    void update(node *t) {
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

    void toggle(node *t) {
        std::swap(t->l, t->r);
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

    node *rotate_right(node *t) {
        node *l = t->l;
        t->l = l->r;
        l->r = t;

        update(t);
        update(l);

        return l;
    }

    node *rotate_left(node *t) {
        node *r = t->r;
        t->r = r->l;
        r->l = t;

        update(t);
        update(r);

        return r;
    }

    node *splay(node *t, i32 k) {
        push(t);

        const i32 lsz = size(t->l);
        if (k == lsz) return t;

        if (k < lsz) {
            t->l = splay(t->l, k);
            t = rotate_right(t);
        } else {
            t->r = splay(t->r, k - lsz - 1);
            t = rotate_left(t);
        }

        update(t);
        return t;
    }

    node *merge(node *l, node *r) {
        if (l == nullptr || r == nullptr) return l != nullptr ? l : r;

        r = splay(r, 0);
        r->l = l;
        update(r);

        return r;
    }

    std::pair<node *, node *> split(node *&root, i32 k) {
        if (k >= size(root)) return {root, nullptr};

        root = splay(root, k);
        node *l = root->l;
        root->l = nullptr;

        update(root);
        return {l, root};
    }

    std::tuple<node *, node *, node *> split3(node *&root, i32 l, i32 r) {
        if (l == 0) {
            auto [b, c] = split(root, r);
            return {nullptr, b, c};
        }

        root = splay(root, l - 1);
        auto [b, c] = split(root->r, r - l);
        root->r = nullptr;

        update(root);
        return {root, b, c};
    }

    node *merge3(node *a, node *b, node *c) {
        node *t = merge(b, c);
        if (a == nullptr) return t;

        a->r = t;
        update(a);

        return a;
    }

    void insert(node *&root, i32 p, X x) { insert(root, p, make_node(x)); }

    void insert(node *&root, i32 p, node *t) {
        assert(0 <= p && p <= size(root));

        if (p == size(root)) {
            t->l = root;
            update(t);
            root = t;
        } else if (p == 0) {
            t->r = root;
            update(t);
            root = t;
        } else {
            root = splay(root, p);
            t->l = root->l;
            root->l = t;

            update(t);
            update(root);
        }
    }

    void erase(node *&root, i32 p) {
        assert(0 <= p && p < size(root));

        root = splay(root, p);
        root = merge(root->l, root->r);
    }

    void set(node *&root, i32 p, X x) {
        assert(0 <= p && p < size(root));

        root = splay(root, p);
        root->val = x;

        update(root);
    }

    X prod(node *&root, i32 l, i32 r) {
        assert(0 <= l && l <= r && r <= size(root));

        if (l == r) return MX::unit();

        auto [a, b, c] = split3(root, l, r);

        X x = b->sum;
        root = merge3(a, b, c);

        return x;
    }

    X get(node *&root, i32 p) {
        assert(0 <= p && p < size(root));

        root = splay(root, p);
        return root->val;
    }

    void dump(node *root, std::vector<X> &v) {
        if (root == nullptr) return;

        push(root);
        dump(root->l, v);
        v.push_back(root->val);
        dump(root->r, v);
    }

    std::vector<X> get_all(node *&root) {
        std::vector<X> v;
        v.reserve(size(root));

        dump(root, v);
        return v;
    }

    void multiply(node *&root, i32 p, X x) {
        assert(0 <= p && p < size(root));

        root = splay(root, p);
        root->val = MX::op(root->val, x);

        update(root);
    }

    void reverse(node *&root, i32 l, i32 r) {
        assert(0 <= l && l <= r && r <= size(root));

        if (l + 1 >= r) return;

        auto [a, b, c] = split3(root, l, r);
        toggle(b);

        root = merge3(a, b, c);
    }
};

#endif // LIB_SPLAY_TREE_HPP
