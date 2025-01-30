#ifndef LIB_BASIC_SPLAY_TREE_HPP
#define LIB_BASIC_SPLAY_TREE_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>

template <typename T>
struct basic_splay_tree {
    struct node {
        node *l, *r;
        T val;
        bool rev;
        i32 sz;

        explicit node(const T &x)
            : l{nullptr}, r{nullptr}, val{x}, rev{false}, sz{1} {}

        node()
            : node(T()) {}
    };

    static i32 size(node *t) {
        return t != nullptr ? t->sz : 0;
    }

    i32 n, pid;
    node *pool;

    basic_splay_tree()
        : pool{nullptr} {}

    explicit basic_splay_tree(i32 m)
        : basic_splay_tree() {
        build(m);
    }

    ~basic_splay_tree() {
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

    node *make_node(T x) {
        assert(pid < n);

        return &(pool[pid++] = node(x));
    }

    node *make_nodes(i32 p) {
        return make_nodes(p, [](i32) -> T { return T(); });
    }

    node *make_nodes(const std::vector<T> &v) {
        return make_nodes(static_cast<i32>(v.size()), [&](i32 i) -> T { return v[i]; });
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

private:
    static void update(node *t) {
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

    static node *rotate_right(node *t) {
        node *l = t->l;
        t->l = l->r;
        l->r = t;

        update(t);
        update(l);

        return l;
    }

    static node *rotate_left(node *t) {
        node *r = t->r;
        t->r = r->l;
        r->l = t;

        update(t);
        update(r);

        return r;
    }

    static node *splay(node *t, i32 k) {
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

public:
    static node *merge(node *l, node *r) {
        if (l == nullptr || r == nullptr) return l != nullptr ? l : r;

        r = splay(r, 0);
        r->l = l;
        update(r);

        return r;
    }

    static std::pair<node *, node *> split(node *&root, i32 k) {
        if (k >= size(root)) return {root, nullptr};

        root = splay(root, k);
        node *l = root->l;
        root->l = nullptr;

        update(root);
        return {l, root};
    }

    static std::tuple<node *, node *, node *> split3(node *&root, i32 l, i32 r) {
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

    static node *merge3(node *a, node *b, node *c) {
        node *t = merge(b, c);
        if (a == nullptr) return t;

        a->r = t;
        update(a);

        return a;
    }

    void insert(node *&root, i32 p, const T &x) {
        insert(root, p, make_node(x));
    }

    static void insert(node *&root, i32 p, node *t) {
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

    static void set(node *&root, i32 p, const T &x) {
        assert(0 <= p && p < size(root));

        root = splay(root, p);
        root->val = x;

        update(root);
    }

    static T get(node *&root, i32 p) {
        assert(0 <= p && p < size(root));

        root = splay(root, p);
        return root->val;
    }

    static void reverse(node *&root, i32 l, i32 r) {
        assert(0 <= l && l <= r && r <= size(root));

        if (l + 1 >= r) return;

        auto [a, b, c] = split3(root, l, r);
        toggle(b);

        root = merge3(a, b, c);
    }

    static void dump(node *root, std::vector<T> &v) {
        if (root == nullptr) return;

        push(root);
        dump(root->l, v);
        v.push_back(root->val);
        dump(root->r, v);
    }

    static std::vector<T> get_all(node *&root) {
        std::vector<T> v;
        v.reserve(size(root));

        dump(root, v);
        return v;
    }
};

#endif // LIB_BASIC_SPLAY_TREE_HPP
