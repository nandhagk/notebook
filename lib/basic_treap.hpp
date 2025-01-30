#ifndef LIB_BASIC_TREAP_HPP
#define LIB_BASIC_TREAP_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/random.hpp>
#include <lib/type_traits.hpp>

template <typename T>
struct basic_treap {
    using X = T;

    struct node {
        node *l, *r;
        X val;
        bool rev;
        i32 sz;
        u64 pr;

        explicit node(const X &x)
            : l{nullptr}, r{nullptr}, val{x}, rev{false}, sz{1}, pr{MT()} {}

        node()
            : node(T()) {}
    };

    static i32 size(node *t) {
        return t != nullptr ? t->sz : 0;
    }

    i32 n, pid;
    node *pool;

    basic_treap()
        : pool{nullptr} {}

    explicit basic_treap(i32 m)
        : basic_treap() {
        build(m);
    }

    ~basic_treap() {
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

    node *make_node(X x) {
        assert(pid < n);

        return &(pool[pid++] = node(x));
    }

    node *make_nodes(i32 p) {
        return make_nodes(p, [](i32) -> T { return T(); });
    }

    node *make_nodes(const std::vector<X> &v) {
        return make_nodes(static_cast<i32>(v.size()), [&](i32 i) -> X { return v[i]; });
    }

    template <typename F>
    node *make_nodes(i32 p, F f) {
        std::vector<node *> nodes(p);
        for (i32 i = 0; i < p; ++i) nodes[i] = make_node(f(i));

        return assign(nodes);
    }

    node *assign(const std::vector<node *> &v) {
        if (v.empty()) return make_node();

        const i32 p = static_cast<i32>(v.size());

        std::vector<node *> st;
        st.reserve(p);

        for (i32 i = 0; i < p; ++i) {
            node *last = nullptr;
            while (!st.empty() && v[i]->pr >= st.back()->pr) {
                last = st.back();
                update(last);
                st.pop_back();
            }

            v[i]->l = last;

            if (!st.empty()) st.back()->r = v[i];
            st.push_back(v[i]);
        }

        for (i32 i = static_cast<i32>(st.size()) - 1; i >= 0; --i) update(st[i]);
        return st[0];
    }

private:
    static node *update(node *t) {
        t->sz = 1;
        t->mus = t->sum = t->val;

        if (t->l != nullptr) t->sz += t->l->sz;
        if (t->r != nullptr) t->sz += t->r->sz;

        return t;
    }

    static void toggle(node *t) {
        std::swap(t->l, t->r);
        t->rev ^= true;
    }

    static void push(node *t) {
        if (t->rev) {
            if (t->l != nullptr) toggle(t->l);
            if (t->r != nullptr) toggle(t->r);
            t->rev = false;
        }
    }

public:
    static node *merge(node *l, node *r) {
        if (l == nullptr || r == nullptr) return l != nullptr ? l : r;

        if (l->pr > r->pr) {
            push(l);
            l->r = merge(l->r, r);
            return update(l);
        } else {
            push(r);
            r->l = merge(l, r->l);
            return update(r);
        }
    }

    static std::pair<node *, node *> split(node *&root, i32 k) {
        if (root == nullptr) return {nullptr, nullptr};

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

    void insert(node *&root, i32 p, const X &x) {
        assert(0 <= p && p <= size(root));

        insert(root, p, make_node(x));
    }

    static void insert(node *&root, i32 p, node *t) {
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

    static void set(node *&root, i32 p, const X &x) {
        assert(0 <= p && p < size(root));

        auto [l, r] = split(root, p);
        auto [a, b] = split(r, 1);

        *a = node(x);
        root = merge(l, merge(a, b));
    }

    static X get(node *&root, i32 p) {
        assert(0 <= p && p < size(root));

        return prod(root, p, p + 1);
    }

    static void reverse(node *&root) {
        toggle(root);
    }

    static void reverse(node *&root, i32 l, i32 r) {
        assert(0 <= l && l <= r && r <= size(root));

        if (l == r) return;

        auto [x, y] = split(root, r);
        auto [p, q] = split(x, l);

        reverse(q);
        root = merge(merge(p, q), y);
    }

    static void dump(node *root, std::vector<X> &v) {
        if (root == nullptr) return;

        push(root);
        dump(root->l, v);
        v.push_back(root->val);
        dump(root->r, v);
    }

    static std::vector<X> get_all(node *&root) {
        std::vector<X> v;
        v.reserve(size(root));

        dump(root, v);
        return v;
    }
};

#endif // LIB_BASIC_TREAP_HPP
