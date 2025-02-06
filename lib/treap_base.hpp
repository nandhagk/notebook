#ifndef LIB_TREAP_BASE_HPP
#define LIB_TREAP_BASE_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/random.hpp>
#include <lib/type_traits.hpp>

template <typename Node>
struct treap_base {
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

    treap_base()
        : pool{nullptr} {}

    explicit treap_base(i32 m)
        : treap_base() {
        build(m);
    }

    ~treap_base() {
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
        std::vector<np> nodes(p);
        for (i32 i = 0; i < p; ++i) nodes[i] = make_node(f(i));

        return assign(nodes);
    }

    np assign(const std::vector<np> &v) {
        if (v.empty()) return make_node();

        const i32 p = static_cast<i32>(v.size());

        std::vector<np> st;
        st.reserve(p);

        for (i32 i = 0; i < p; ++i) {
            np last = nullptr;
            while (!st.empty() && v[i]->pr >= st.back()->pr) {
                last = st.back();
                last->update();
                st.pop_back();
            }

            v[i]->l = last;

            if (!st.empty()) st.back()->r = v[i];
            st.push_back(v[i]);
        }

        for (i32 i = static_cast<i32>(st.size()) - 1; i >= 0; --i) st[i]->update();
        return st[0];
    }

    static np merge(np l, np r) {
        if (l == nullptr || r == nullptr) return l != nullptr ? l : r;

        if (l->pr > r->pr) {
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

    void insert(np &root, i32 p, const X &x) {
        assert(0 <= p && p <= size(root));

        insert(root, p, make_node(x));
    }

    static void insert(np &root, i32 p, np t) {
        assert(0 <= p && p <= size(root));

        auto [l, r] = split(root, p);
        root = merge(l, merge(t, r));
    }

    void erase(np &root, i32 p) {
        assert(0 <= p && p < size(root));

        auto [l, r] = split(root, p);
        auto [_, b] = split(r, 1);
        root = merge(l, b);
    }

    static void set(np &root, i32 p, const X &x) {
        assert(0 <= p && p < size(root));

        auto [l, r] = split(root, p);
        auto [a, b] = split(r, 1);

        *a = node(x);
        root = merge(l, merge(a, b));
    }

    static void multiply(np &root, i32 p, const X &x) {
        assert(0 <= p && p < size(root));

        auto [l, r] = split(root, p);
        auto [a, b] = split(r, 1);

        *a = node(MX::op(a->val, x));
        root = merge(l, merge(a, b));
    }

    static void apply(np &root, i32 l, i32 r, const A &a) {
        assert(0 <= l && l <= r && r <= size(root));

        if (l == r) return;

        auto [x, y] = split(root, l);
        auto [p, q] = split(y, r - l);

        p->all_apply(a);
        root = merge(x, merge(p, q));
    }

    static X prod(np &root, i32 l, i32 r) {
        assert(0 <= l && l <= r && r <= size(root));

        if (l == r) return MX::unit();

        auto [x, y] = split(root, l);
        auto [p, q] = split(y, r - l);

        if (p != nullptr) p->push();
        X v = p->sum;

        root = merge(x, merge(p, q));
        return v;
    }

    static X get(np &root, i32 p) {
        assert(0 <= p && p < size(root));

        if constexpr (is_monoid_v<MX>) return prod(root, p, p + 1);

        auto [x, y] = split(root, p);
        auto [a, b] = split(y, 1);

        if (a != nullptr) a->push();
        X v = a->val;

        root = merge(x, merge(a, b));
        return v;
    }

    static void reverse(np &root) {
        root->toggle();
    }

    static void reverse(np &root, i32 l, i32 r) {
        assert(0 <= l && l <= r && r <= size(root));

        if (l == r) return;

        auto [x, y] = split(root, r);
        auto [p, q] = split(x, l);

        reverse(q);
        root = merge(merge(p, q), y);
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

#endif // LIB_TREAP_BASE_HPP
