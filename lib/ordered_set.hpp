#ifndef LIB_ORDERED_SET_HPP
#define LIB_ORDERED_SET_HPP 1

#include <array>
#include <cassert>
#include <optional>
#include <vector>

#include <lib/prelude.hpp>

template <typename T>
struct ordered_set_small {
    static constexpr i32 max_size = 16;

    i32 size;
    std::array<T, max_size> d;

    ordered_set_small()
        : size(0) {}

    T get(i32 k) const {
        assert(0 <= k && k < size);

        return d[k];
    }

    bool insert(T x) {
        i32 i = 0;

        while (i < size && d[i] < x) ++i;
        if (i < size && d[i] == x) return false;

        for (i32 j = size; j > i; j--) d[j] = d[j - 1];
        d[i] = x;

        ++size;
        return true;
    }

    bool erase(T x) {
        i32 i = 0;

        while (i < size && d[i] < x) ++i;
        if (i == size || d[i] != x) return false;

        for (i32 j = i; j + 1 < size; ++j) d[j] = d[j + 1];

        --size;
        return true;
    }

    ordered_set_small split_half() {
        assert(size == max_size);

        const i32 r = max_size / 2;
        ordered_set_small res;
        for (i32 i = r; i < max_size; ++i) res.d[i - r] = d[i];

        res.size = max_size - r;
        size = r;

        return res;
    }

    i32 lower_bound(T x) const {
        for (i32 i = 0; i < size; ++i)
            if (d[i] >= x) return i;

        return size;
    }

    i32 upper_bound(T x) const {
        for (i32 i = 0; i < size; ++i)
            if (d[i] > x) return i;

        return size;
    }

    T min() const {
        return d[0];
    }

    T max() const {
        return d[size - 1];
    }
};

template <typename T>
struct ordered_set {
private:
    struct node {
        i32 h, sz;
        ordered_set_small<T> x;
        node *l, *r;

        node()
            : h(1), sz(0), l(nullptr), r(nullptr) {}

        explicit node(ordered_set_small<T> z)
            : h(1), sz(z.size), x(z), l(nullptr), r(nullptr) {}

        i32 balanace_factor() const {
            return (l ? l->h : 0) - (r ? r->h : 0);
        }
    };

    static i32 size(node *v) {
        return v ? v->sz : 0;
    }

    static void update(node *v) {
        v->h = std::max(v->l ? v->l->h : 0, v->r ? v->r->h : 0) + 1;
        v->sz = v->x.size;

        if (v->l) v->sz += v->l->sz;
        if (v->r) v->sz += v->r->sz;
    }

    void clear(node *v) {
        if (v == nullptr) return;

        clear(v->l);
        clear(v->r);

        delete v;
    }

    node *rotate_right(node *v) {
        node *l = v->l;
        v->l = l->r;
        l->r = v;
        update(v);
        update(l);
        return l;
    }

    node *rotate_left(node *v) {
        node *r = v->r;
        v->r = r->l;
        r->l = v;
        update(v);
        update(r);
        return r;
    }

    node *balance(node *v) {
        const i32 bf = v->balanace_factor();
        assert(-2 <= bf && bf <= 2);

        if (bf == 2) {
            if (v->l->balanace_factor() == -1) {
                v->l = rotate_left(v->l);
                update(v);
            }

            return rotate_right(v);
        } else if (bf == -2) {
            if (v->r->balanace_factor() == 1) {
                v->r = rotate_right(v->r);
                update(v);
            }

            return rotate_left(v);
        }

        return v;
    }

    node *insert_leftmost(node *v, node *u) {
        if (!v) return u;
        v->l = insert_leftmost(v->l, u);
        update(v);
        return balance(v);
    }

    node *cut_leftmost(node *v, node *&u) {
        if (!v->l) {
            u = v;
            return v->r;
        }

        v->l = cut_leftmost(v->l, u);
        update(v);
        return balance(v);
    }

public:
    node *root;
    ordered_set()
        : root(nullptr) {}

    explicit ordered_set(const std::vector<T> &d)
        : root(nullptr) {
        if (d.empty()) return;

        const i32 N = static_cast<i32>(d.size());
        const i32 B = 12;
        const i32 M = (N + B - 1) / B;

        std::vector<node *> nodes;
        nodes.reserve(M);

        for (i32 i = 0; i < M; ++i) {
            const i32 l = B * i, r = std::min(N, l + B);
            ordered_set_small<T> x;
            for (i32 j = l; j < r; ++j) {
                assert(!j || d[j - 1] < d[j]);
                x.d[j - l] = d[j];
            }

            x.size = r - l;
            nodes.push_back(new node(x));
        }

        const auto rec = [&](auto &&dfs, i32 l, i32 r) -> node * {
            i32 m = (l + r) / 2;
            if (l < m) nodes[m]->l = dfs(dfs, l, m);
            if (m + 1 < r) nodes[m]->r = dfs(dfs, m + 1, r);
            update(nodes[m]);
            return nodes[m];
        };

        root = rec(rec, 0, static_cast<i32>(nodes.size()));
    }

    ~ordered_set() {
        clear();
    }

    i32 size() const {
        return size(root);
    }

    bool insert(T x) {
        bool res = false;
        const auto rec = [&](auto &&dfs, node *v) -> node * {
            if (!v) {
                v = new node();
                v->x.d[0] = x;
                v->x.size = 1;
                res = true;
                update(v);
                return v;
            }

            if (v->l && x < v->x.min()) {
                v->l = dfs(dfs, v->l);
            } else if (v->r && x > v->x.max()) {
                v->r = dfs(dfs, v->r);
            } else {
                res = v->x.insert(x);
                if (v->x.size == ordered_set_small<T>::max_size) {
                    node *u = new node(v->x.split_half());
                    update(u);
                    v->r = insert_leftmost(v->r, u);
                }
            }

            update(v);
            return balance(v);
        };

        root = rec(rec, root);
        return res;
    }

    bool erase(T x) {
        bool res = false;
        const auto rec = [&](auto &&dfs, node *v) -> node * {
            if (!v) return nullptr;

            if (x < v->x.min()) {
                v->l = dfs(dfs, v->l);
            } else if (x > v->x.max()) {
                v->r = dfs(dfs, v->r);
            } else {
                res = v->x.erase(x);
                if (v->x.size == 0) {
                    if (!v->r || !v->l) {
                        return (!v->r ? v->l : v->r);
                    } else {
                        node *u = nullptr;
                        node *r = cut_leftmost(v->r, u);
                        u->l = v->l;
                        u->r = r;
                        update(u);
                        return balance(u);
                    }
                }
            }

            update(v);
            return balance(v);
        };

        root = rec(rec, root);
        return res;
    }

    bool find(T x) const {
        node *v = root;

        while (v) {
            if (x < v->x.min()) {
                v = v->l;
            } else if (x > v->x.max()) {
                v = v->r;
            } else {
                const i32 idx = v->x.lower_bound(x);
                return idx < v->x.size && v->x.get(idx) == x;
            }
        }

        return false;
    }

    std::optional<T> find_prev(T x) const {
        T res = x;

        node *v = root;
        while (v) {
            if (x <= v->x.min()) {
                v = v->l;
            } else if (x > v->x.max()) {
                res = v->x.max();
                v = v->r;
            } else {
                const i32 idx = v->x.lower_bound(x);
                assert(idx);

                return v->x.get(idx - 1);
            }
        }

        if (res == x) return std::nullopt;
        return res;
    }

    std::optional<T> find_next(T x) const {
        T res = x;

        node *v = root;
        while (v) {
            if (x < v->x.min()) {
                res = v->x.min();
                v = v->l;
            } else if (x >= v->x.max()) {
                v = v->r;
            } else {
                const i32 idx = v->x.upper_bound(x);
                assert(idx != v->x.size);

                return v->x.get(idx);
            }
        }

        if (res == x) return std::nullopt;
        return res;
    }

    std::optional<T> find_by_order(i32 k) const {
        if (size() <= k) return std::nullopt;

        node *v = root;
        for (;;) {
            i32 lsize = size(v->l);
            if (k < lsize) {
                v = v->l;
            } else if (k < lsize + v->x.size) {
                return v->x.get(k - lsize);
            } else {
                k -= lsize + v->x.size;
                v = v->r;
            }
        }
    }

    i32 order_of_key(T r) const {
        i32 res = 0;

        node *v = root;
        while (v) {
            if (r <= v->x.min()) {
                v = v->l;
            } else if (r <= v->x.max() + 1) {
                res += size(v->l);
                return res + v->x.lower_bound(r);
            } else {
                res += size(v->l) + v->x.size;
                v = v->r;
            }
        }

        return res;
    }

    std::vector<T> get_all() const {
        if (!root) return {};

        std::vector<T> res;
        const auto rec = [&](auto &&dfs, node *v) -> void {
            if (v->l) dfs(dfs, v->l);
            for (i32 i = 0; i < v->x.size; ++i) res.push_back(v->x.get(i));
            if (v->r) dfs(dfs, v->r);
        };

        rec(rec, root);
        return res;
    }

    void clear() {
        // clear(root);
    }
};

#endif // LIB_ORDERED_SET_HPP
