#ifndef LIB_SPLAY_TREE_BASE_HPP
#define LIB_SPLAY_TREE_BASE_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

// Can be O(n) sometimes (not amortized just for all operations with cut and paste)?!
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
        t->l = assign(l, m, f);
        t->r = assign(m + 1, r, f);

        t->update();
        return t;
    }

private:
    static np rotate_right(np t) {
        np l = t->l;
        t->l = l->r;
        l->r = t;

        t->update();
        l->update();

        return l;
    }

    static np rotate_left(np t) {
        np r = t->r;
        t->r = r->l;
        r->l = t;

        t->update();
        r->update();

        return r;
    }

    static np splay(np t, i32 k) {
        if (t == nullptr) return t;

        t->push();

        const i32 lsz = size(t->l);
        if (k == lsz) return t;

        if (k < lsz) {
            t->l = splay(t->l, k);
            t = rotate_right(t);
        } else {
            t->r = splay(t->r, k - lsz - 1);
            t = rotate_left(t);
        }

        t->update();
        return t;
    }

public:
    static np merge(np l, np r) {
        if (l == nullptr || r == nullptr) return l != nullptr ? l : r;

        r = splay(r, 0);
        r->l = l;
        r->update();

        return r;
    }

    static std::pair<np, np> split(np &root, i32 k) {
        if (k >= size(root)) return {root, nullptr};

        root = splay(root, k);
        np l = root->l;
        root->l = nullptr;

        root->update();
        return {l, root};
    }

    static std::tuple<np, np, np> split3(np &root, i32 l, i32 r) {
        if (l == 0) {
            auto [b, c] = split(root, r);
            return {nullptr, b, c};
        }

        root = splay(root, l - 1);
        auto [b, c] = split(root->r, r - l);
        root->r = nullptr;

        root->update();
        return {root, b, c};
    }

    static np merge3(np a, np b, np c) {
        np t = merge(b, c);
        if (a == nullptr) return t;

        a->r = t;
        a->update();

        return a;
    }

    void insert(np &root, i32 p, const X &x) {
        insert(root, p, make_node(x));
    }

    static void insert(np &root, i32 p, np t) {
        assert(0 <= p && p <= size(root));

        if (p == size(root)) {
            t->l = root;
            t->update();
            root = t;
        } else if (p == 0) {
            t->r = root;
            t->update();
            root = t;
        } else {
            root = splay(root, p);
            t->l = root->l;
            root->l = t;

            t->update();
            root->update();
        }
    }

    void erase(np &root, i32 p) {
        assert(0 <= p && p < size(root));

        root = splay(root, p);
        root = merge(root->l, root->r);
    }

    static void set(np &root, i32 p, const X &x) {
        assert(0 <= p && p < size(root));

        root = splay(root, p);
        root->val = x;

        root->update();
    }

    static void apply(np &root, i32 l, i32 r, const A &a) {
        assert(0 <= l && l <= r && r <= size(root));

        if (l == r) return;

        auto [x, y, z] = split3(root, l, r);
        y->all_apply(a);

        root = merge3(x, y, z);
    }

    static X prod(np &root, i32 l, i32 r) {
        assert(0 <= l && l <= r && r <= size(root));

        if (l == r) return MX::unit();

        auto [a, b, c] = split3(root, l, r);

        X x = b->sum;
        root = merge3(a, b, c);

        return x;
    }

    static void multiply(np &root, i32 p, const X &x) {
        assert(0 <= p && p < size(root));

        root = splay(root, p);
        root->val = MX::op(root->val, x);

        root->update();
    }

    static X get(np &root, i32 p) {
        assert(0 <= p && p < size(root));

        root = splay(root, p);
        return root->val;
    }

    static void reverse(np &root, i32 l, i32 r) {
        assert(0 <= l && l <= r && r <= size(root));

        if (l == r) return;

        auto [a, b, c] = split3(root, l, r);
        b->toggle();

        root = merge3(a, b, c);
    }

    static void dump(np root, std::vector<X> &v) {
        if (root == nullptr) return;

        root->push();
        dump(root->l, v);
        v.push_back(root->val);
        dump(root->r, v);
    }

    static std::vector<X> get_all(np &root) {
        std::vector<X> v;
        v.reserve(size(root));

        dump(root, v);
        return v;
    }
};

#endif // LIB_SPLAY_TREE_BASE_HPP
