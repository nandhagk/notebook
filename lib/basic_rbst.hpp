#ifndef LIB_BASIC_RBST_HPP
#define LIB_BASIC_RBST_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/random.hpp>

template <typename T>
struct basic_rbst {
    struct node {
        node *l, *r;
        T val;
        bool rev;
        u32 sz;

        explicit node(T x) : l{nullptr}, r{nullptr}, val{x}, rev{false}, sz{1} {}

        node() : node(T()) {}
    };

    i32 size(node *t) const {
        return t != nullptr ? static_cast<i32>(t->sz) : 0;
    }

    i32 n, pid;
    node *pool;

    basic_rbst() : pool{nullptr} {}

    explicit basic_rbst(i32 m) : basic_rbst() {
        build(m);
    }

    ~basic_rbst() {
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

    node *update(node *t) {
        t->sz = 1;
        if (t->l != nullptr) t->sz += t->l->sz;
        if (t->r != nullptr) t->sz += t->r->sz;

        return t;
    }

    void toggle(node *t) {
        std::swap(t->l, t->r);
        t->rev ^= true;
    }

    void push(node *t) {
        if (t->rev) {
            if (t->l != nullptr) toggle(t->l);
            if (t->r != nullptr) toggle(t->r);
            t->rev = false;
        }
    }

    node *merge(node *l, node *r) {
        if (l == nullptr || r == nullptr) return l != nullptr ? l : r;

        if (MT() % (l->sz + r->sz) < l->sz) {
            push(l);
            l->r = merge(l->r, r);
            return update(l);
        } else {
            push(r);
            r->l = merge(l, r->l);
            return update(r);
        }
    }

    std::pair<node *, node *> split(node *&root, i32 k) {
        if (k >= size(root)) return {root, nullptr};

        push(root);
        if (const i32 lsz = size(root->l); k > lsz) {
            auto [l, r] = split(root->r, k - lsz - 1);
            root->r = l;
            return {update(root), r};
        } else {
            auto [l, r] = split(root->l, k);
            root->l = r;
            return {l, update(root)};
        }
    }

    void insert(node *&root, i32 p, T x) {
        assert(0 <= p && p <= size(root));

        insert(root, p, make_node(x));
    }

    void insert(node *&root, i32 p, node *t) {
        assert(0 <= p && p <= size(root));

        auto [l, r] = split(root, p);
        root = merge(l, merge(t, r));
    }

    void erase(node *&root, i32 p) {
        assert(0 <= p && p < size(root));

        auto [l, r] = split(root, p);
        auto [_, b] = split(r, 1);
        root = merge(l, b);
    }

    void set(node *&root, i32 p, T x) {
        assert(0 <= p && p < size(root));

        auto [l, r] = split(root, p);
        auto [a, b] = split(r, 1);

        a->val = x;
        root = merge(l, merge(a, b));
    }

    T get(node *&root, i32 p) {
        assert(0 <= p && p < size(root));

        return prod(root, p, p + 1);
    }

    void dump(node *root, std::vector<T> &v) {
        if (root == nullptr) return;

        push(root);
        dump(root->l, v);
        v.push_back(root->val);
        dump(root->r, v);
    }

    std::vector<T> get_all(node *&root) {
        std::vector<T> v;
        v.reserve(size(root));

        dump(root, v);
        return v;
    }

    void reverse(node *&root) {
        toggle(root);
    }

    void reverse(node *&root, i32 l, i32 r) {
        assert(0 <= l && l <= r && r <= size(root));

        if (l + 1 >= r) return;

        auto [x, y] = split(root, r);
        auto [p, q] = split(x, l);

        reverse(q);
        root = merge(merge(p, q), y);
    }
};

#endif // LIB_BASIC_RBST_HPP
