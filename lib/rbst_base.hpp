#ifndef LIB_RBST_BASE_HPP
#define LIB_RBST_BASE_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/random.hpp>
#include <lib/type_traits.hpp>

template <typename Node>
struct rbst_base {
    using MX = typename Node::MX;
    using MA = typename Node::MA;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    using np = Node *;

    static i32 size(np t) {
        return t != nullptr ? t->sz : 0;
    }

    i32 n, pid;
    np pool;

    rbst_base()
        : pool{nullptr} {}

    explicit rbst_base(i32 m)
        : rbst_base() {
        build(m);
    }

    ~rbst_base() {
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

    np make_node(X x) {
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

    static np merge(np l, np r) {
        if (l == nullptr || r == nullptr) return l != nullptr ? l : r;

        if (static_cast<i32>(MT() % (l->sz + r->sz)) < l->sz) {
            l->push();
            l->r = merge(l->r, r);
            l->update();

            return l;
        } else {
            r->push();
            r->l = merge(l, r->l);
            r->update();

            return r;
        }
    }

    static std::pair<np, np> split(np &root, i32 k) {
        if (root == nullptr) return {nullptr, nullptr};

        root->push();
        if (const i32 lsz = size(root->l); k > lsz) {
            auto [l, r] = split(root->r, k - lsz - 1);
            root->r = l;
            root->update();

            return {root, r};
        } else {
            auto [l, r] = split(root->l, k);
            root->l = r;
            root->update();

            return {l, root};
        }
    }

    static std::tuple<np, np, np> split3(np &root, i32 l, i32 r) {
        auto [a, b] = split(root, l);
        auto [c, d] = split(b, r - l);

        return {a, c, d};
    }

    static np merge3(np a, np b, np c) {
        return merge(a, merge(b, c));
    }

    void insert(np &root, i32 p, const X &x) {
        assert(0 <= p && p <= size(root));

        insert(root, p, make_node(x));
    }

    static void insert(np &root, i32 p, np t) {
        assert(0 <= p && p <= size(root));

        auto [l, r] = split(root, p);
        root = merge3(l, t, r);
    }

    void erase(np &root, i32 p) {
        assert(0 <= p && p < size(root));

        auto [l, _, r] = split3(root, p, p + 1);
        root = merge(l, r);
    }

    static void set(np &root, i32 p, const X &x) {
        assert(0 <= p && p < size(root));

        auto [a, b, c] = split3(root, p, p + 1);

        *b = node(x);
        root = merge3(a, b, c);
    }

    static void multiply(np &root, i32 p, const X &x) {
        assert(0 <= p && p < size(root));

        auto [a, b, c] = split3(root, p, p + 1);

        *b = node(MX::op(b->val, x));
        root = merge3(a, b, c);
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

        if (b != nullptr) b->push();
        X v = b->sum;

        root = merge3(a, b, c);
        return v;
    }

    static X get(np &root, i32 p) {
        assert(0 <= p && p < size(root));

        auto [a, b, c] = split3(root, p, p + 1);

        if (b != nullptr) b->push();
        X v = b->val;

        root = merge3(a, b, c);
        return v;
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

    static std::vector<X> get_all(np root) {
        std::vector<X> v;
        v.reserve(size(root));

        dump(root, v);
        return v;
    }
};

#endif // LIB_RBST_BASE_HPP
