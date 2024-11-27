#ifndef LIB_LAZY_RBST_HPP
#define LIB_LAZY_RBST_HPP 1

#include <vector>
#include <cassert>

#include <lib/prelude.hpp>
#include <lib/random.hpp>

template <typename ActedMonoid>
struct lazy_rbst {
        using AM = ActedMonoid;

        using MX = typename AM::MX;
        using MA = typename AM::MA;

        using X = typename MX::ValueT;
        using A = typename MA::ValueT;

        struct node {
                node *l, *r;
                X val, sum;
                A lz;
                bool rev;
                u32 sz;

                explicit node(const X &x):
                        l{nullptr}, r{nullptr}, val{x}, sum{x}, lz{MA::unit()}, rev{false}, sz{1} {}

                node():
                        node(MX::unit()) {}
        };

        i32 size(node* t) const {
                return t != nullptr ? static_cast<i32>(t->sz) : 0;
        }

        i32 n, pid;
        node* pool;

        lazy_rbst():
                pool{nullptr} {}

        explicit lazy_rbst(i32 m):
                lazy_rbst() {
                build(m);
        }

        ~lazy_rbst() {
                reset();
        }
                
        void build(i32 m) {
                reset();

                n = m;
                pool = new node[n];
        }

        void reset() {
                pid = 0;
                delete [] pool;
        }

        node* make_node() {
                return nullptr;
        }

        node* make_node(const X& x) {
                assert(pid < n);

                return &(pool[pid++] = node(x));
        }

        node* make_nodes(i32 p) {
                return make_nodes(p, [](i32) -> X { return MX::unit(); });
        }

        node* make_nodes(const std::vector<X> &v) {
                return make_nodes(static_cast<i32>(v.size()), [&](i32 i) -> X { return v[i]; });
        }

        template <typename F>
        node* make_nodes(i32 p, F f) {
                return assign(0, p, f);
        }

        template <typename F>
        node* assign(i32 l, i32 r, F f) {
                if (l == r) return make_node();
                if (l + 1 == r) return make_node(f(l));

                const i32 m = (l + r) / 2;

                node* t = make_node(f(m));
                t->l = assign(l, m, f);
                t->r = assign(m + 1, r, f);

                update(t);
                return t;
        }

        node* update(node* t) {
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

                return t;
        }

        void all_apply(node* t, const A& a) {
                t->val = AM::act(t->val, a, 1);
                t->sum = AM::act(t->sum, a, t->sz);
                t->lz = MA::op(t->lz, a);
        }

        void toggle(node* t) {
                std::swap(t->l, t->r);
                t->rev ^= true;
        }

        void push(node* t) {
                if (t->lz != MA::unit()) {
                        if (t->l != nullptr) all_apply(t->l, t->lz);
                        if (t->r != nullptr) all_apply(t->r, t->lz);
                        t->lz = MA::unit();
                }

                if (t->rev) {
                        if (t->l != nullptr) toggle(t->l);
                        if (t->r != nullptr) toggle(t->r);
                        t->rev = false;
                }
        }

        node* merge(node* l, node* r) {
                if (l == nullptr || r == nullptr) return l != nullptr ? l : r;

                if ((MT() % (l->sz + r->sz)) < l->sz) {
                        push(l);
                        l->r = merge(l->r, r);
                        return update(l);
                } else {
                        push(r);
                        r->l = merge(l, r->l);
                        return update(r);
                }
        }

        std::pair<node*, node*> split(node* &root, i32 k) {
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

        void insert(node* &root, i32 p, const X &x) {
                assert(0 <= p && p <= size(root));

                insert(root, p, make_node(x));
        }

        void insert(node* &root, i32 p, node* t) {
                assert(0 <= p && p <= size(root));

                auto [l, r] = split(root, p);
                root = merge(l, merge(t, r));
        }

        void erase(node* &root, i32 p) {
                assert(0 <= p && p < size(root));

                auto [l, r] = split(root, p);
                auto [_, b] = split(r, 1);
                root = merge(l, b);
        }

        void set(node* &root, i32 p, const X &x) {
                assert(0 <= p && p < size(root));

                auto [l, r] = split(root, p);
                auto [a, b] = split(r, 1);

                *a = node(x);
                root = merge(l, merge(a, b));
        }

        void apply(node* &root, i32 l, i32 r, const A& a) {
                assert(0 <= l && l <= r && r <= size(root));

                if (l == r) return;

                auto [x, y] = split(root, l);
                auto [p, q] = split(y, r - l);

                all_apply(p, a);
                root = merge(x, merge(p, q));
        }

        X prod(node* &root, i32 l, i32 r) {
                assert(0 <= l && l <= r && r <= size(root));

                if (l == r) return MX::unit();

                auto [x, y] = split(root, l);
                auto [p, q] = split(y, r - l);

                push(p);
                X v = p->sum;

                root = merge(x, merge(p, q));
                return v;
        }

        X get(node* &root, i32 p) {
                assert(0 <= p && p < size(root));

                return prod(root, p, p + 1);
        }

        void dump(node* root, std::vector<X> &v) {
                if (root == nullptr) return;

                push(root);
                dump(root->l, v);
                v.push_back(root->val);
                dump(root->r, v);
        }

        std::vector<X> get_all(node* &root) {
                std::vector<X> v(size(root));
                dump(root, v);

                return v;
        }



        void multiply(node* &root, i32 p, const X &x) {
                assert(0 <= p && p < size(root));

                set(root, p, MX::op(get(p), x));
        }

        void reverse(node* &root) {
                toggle(root);
        }

        void reverse(node* &root, i32 l, i32 r) {
                assert(0 <= l && l <= r && r <= size(root));

                if (l + 1 >= r) return;

                auto [x, y] = split(root, r);
                auto [p, q] = split(x, l);

                reverse(q);
                root = merge(merge(p, q), y);
        }
};

#endif // LIB_LAZY_RBST_HPP
