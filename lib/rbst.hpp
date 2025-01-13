#ifndef LIB_RBST_HPP
#define LIB_RBST_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/random.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_monoid_t<Monoid> * = nullptr>
struct rbst {
    using MX = Monoid;
    using X = typename MX::ValueT;

    struct node {
        node *l, *r;
        X val, sum, mus;
        bool rev;
        u32 sz;

        explicit node(const X &x)
            : l{nullptr}, r{nullptr}, val{x}, sum{x}, mus{x}, rev{false}, sz{1} {}

        node()
            : node(MX::unit()) {}
    };

    i32 size(node *t) const {
        return t != nullptr ? static_cast<i32>(t->sz) : 0;
    }

    i32 n, pid;
    node *pool;

    rbst()
        : pool{nullptr} {}

    explicit rbst(i32 m)
        : rbst() {
        build(m);
    }

    ~rbst() {
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

    node *update(node *t) {
        t->sz = 1;
        t->mus = t->sum = t->val;

        if (t->l != nullptr) {
            t->sz += t->l->sz;
            t->sum = MX::op(t->l->sum, t->sum);
            t->mus = MX::op(t->mus, t->l->mus);
        }

        if (t->r != nullptr) {
            t->sz += t->r->sz;
            t->sum = MX::op(t->sum, t->r->sum);
            t->mus = MX::op(t->r->mus, t->mus);
        }

        return t;
    }

    void toggle(node *t) {
        std::swap(t->l, t->r);
        std::swap(t->sum, t->mus);
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

    void set(node *&root, i32 p, const X &x) {
        assert(0 <= p && p < size(root));

        auto [l, r] = split(root, p);
        auto [a, b] = split(r, 1);

        *a = node(x);
        root = merge(l, merge(a, b));
    }

    X prod(node *&root, i32 l, i32 r) {
        assert(0 <= l && l <= r && r <= size(root));

        if (l == r) return MX::unit();

        auto [x, y] = split(root, l);
        auto [p, q] = split(y, r - l);

        if (p != nullptr) push(p);
        X v = p->sum;

        root = merge(x, merge(p, q));
        return v;
    }

    X get(node *&root, i32 p) {
        assert(0 <= p && p < size(root));

        return prod(root, p, p + 1);
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

    void multiply(node *&root, i32 p, const X &x) {
        assert(0 <= p && p < size(root));

        auto [l, r] = split(root, p);
        auto [a, b] = split(r, 1);

        *a = node(MX::op(a->val, x));
        root = merge(l, merge(a, b));
    }

    void reverse(node *&root) {
        toggle(root);
    }

    void reverse(node *&root, i32 l, i32 r) {
        assert(0 <= l && l <= r && r <= size(root));

        if (l == r) return;

        auto [x, y] = split(root, r);
        auto [p, q] = split(x, l);

        reverse(q);
        root = merge(merge(p, q), y);
    }
};

#endif // LIB_RBST_HPP
