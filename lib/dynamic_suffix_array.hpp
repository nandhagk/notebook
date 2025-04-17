#ifndef LIB_DYNAMIC_SUFFIX_ARRAY_HPP
#define LIB_DYNAMIC_SUFFIX_ARRAY_HPP 1

#include <algorithm>
#include <string>
#include <vector>
#include <cassert>

#include <lib/prelude.hpp>

struct dynamic_suffix_array {
private:
    struct node {
        i32 sa, lcp;
        node *l, *r, *p;
        i32 sz, mi;

        node(i32 sa_, i32 lcp_, node *p_)
            : sa(sa_), lcp(lcp_), l(nullptr), r(nullptr), p(p_), sz(1), mi(lcp) {}

        void update() {
            sz = 1;
            mi = lcp;

            if (l) {
                sz += l->sz;
                mi = std::min(mi, l->mi);
            }

            if (r) {
                sz += r->sz;
                mi = std::min(mi, r->mi);
            }
        }
    };

    node *root;
    std::vector<i64> tag; // tag of a suffix (reversed id)
    std::string s;        // reversed

public:
    dynamic_suffix_array()
        : root(nullptr) {}

    explicit dynamic_suffix_array(std::string t)
        : dynamic_suffix_array() {
        std::reverse(t.begin(), t.end());
        for (char c : t) push_front(c);
    }

    ~dynamic_suffix_array() {
        std::vector<node *> q = {root};

        while (!q.empty()) {
            node *x = q.back();
            q.pop_back();

            if (!x) continue;

            q.push_back(x->l);
            q.push_back(x->r);

            delete x;
        }
    }

private:
    i32 size(node *x) const {
        return x ? x->sz : 0;
    }

    i32 mirror(i32 i) const {
        return static_cast<i32>(s.size()) - 1 - i;
    }

    bool cmp(i32 i, i32 j) const {
        if (s[i] != s[j]) return s[i] < s[j];
        if (i == 0 || j == 0) return i < j;
        return tag[i - 1] < tag[j - 1];
    }

    void fix_path(node *x) {
        while (x) {
            x->update();
            x = x->p;
        }
    }

    void flatten(std::vector<node *> &v, node *x) {
        if (!x) return;

        flatten(v, x->l);
        v.push_back(x);
        flatten(v, x->r);
    }

    void build(std::vector<node *> &v, node *&x, node *p, i32 L, i32 R, i64 l, i64 r) {
        if (L > R) {
            x = nullptr;
            return;
        }

        const i32 M = (L + R) / 2;
        const i64 m = (l + r) / 2;

        x = v[M];
        x->p = p;
        tag[x->sa] = m;

        build(v, x->l, x, L, M - 1, l, m - 1);
        build(v, x->r, x, M + 1, R, m + 1, r);
        x->update();
    }

    void fix(node *&x, node *p, i64 l, i64 r) {
        if (3 * std::max(size(x->l), size(x->r)) <= 2 * size(x)) return x->update();

        std::vector<node *> v;
        flatten(v, x);
        build(v, x, p, 0, static_cast<i32>(v.size()) - 1, l, r);
    }

    node *next(node *x) {
        if (x->r) {
            x = x->r;
            while (x->l) x = x->l;

            return x;
        }

        while (x->p && x->p->r == x) x = x->p;
        return x->p;
    }

    node *prev(node *x) {
        if (x->l) {
            x = x->l;
            while (x->r) x = x->r;

            return x;
        }

        while (x->p && x->p->l == x) x = x->p;
        return x->p;
    }

    i32 get_lcp(node *x, node *y) {
        if (!x || !y) return 0; // change defaut value here

        if (s[x->sa] != s[y->sa]) return 0;
        if (x->sa == 0 || y->sa == 0) return 1;

        return 1 + prod(mirror(x->sa - 1), mirror(y->sa - 1));
    }

    void add_suf(node *&x, node *p, i32 id, i64 l, i64 r) {
        if (!x) {
            x = new node(id, 0, p);

            node *prv = prev(x);
            node *nxt = next(x);

            i32 lcp_cur = get_lcp(prv, x);
            i32 lcp_nxt = get_lcp(x, nxt);
            if (nxt) {
                nxt->lcp = lcp_nxt;
                fix_path(nxt);
            }

            x->lcp = lcp_cur;
            tag[id] = (l + r) / 2;
            x->update();

            return;
        }

        if (cmp(id, x->sa))
            add_suf(x->l, x, id, l, tag[x->sa] - 1);
        else
            add_suf(x->r, x, id, tag[x->sa] + 1, r);

        fix(x, p, l, r);
    }

    void rem_suf(node *&x, i32 id) {
        if (x->sa != id) {
            if (tag[id] < tag[x->sa]) return rem_suf(x->l, id);
            return rem_suf(x->r, id);
        }

        node *nxt = next(x);
        if (nxt) {
            nxt->lcp = std::min(nxt->lcp, x->lcp);
            fix_path(nxt);
        }

        node *p = x->p;
        node *tmp = x;
        if (!x->l || !x->r) {
            x = x->l ? x->l : x->r;
            if (x) x->p = p;
        } else {
            for (tmp = x->l, p = x; tmp->r; tmp = tmp->r) p = tmp;
            x->sa = tmp->sa, x->lcp = tmp->lcp;
            if (tmp->l) tmp->l->p = p;
            if (p->l == tmp)
                p->l = tmp->l;
            else
                p->r = tmp->l;
        }

        fix_path(p);
        delete tmp;
    }

    i32 prod(node *x, i64 l, i64 r, i64 a, i64 b) const {
        if (!x || tag[x->sa] == -1 || r < a || b < l) return static_cast<i32>(s.size());
        if (a <= l && r <= b) return x->mi;

        i32 ans = static_cast<i32>(s.size());

        if (a <= tag[x->sa] && tag[x->sa] <= b) ans = std::min(ans, x->lcp);
        ans = std::min(ans, prod(x->l, l, tag[x->sa] - 1, a, b));
        ans = std::min(ans, prod(x->r, tag[x->sa] + 1, r, a, b));

        return ans;
    }

public:
    void push_front(char c) {
        s += c;
        tag.push_back(-1);

        add_suf(root, nullptr, static_cast<i32>(s.size()) - 1, 0, inf<i64>);
    }

    void pop_front() {
        if (s.empty()) return;

        s.pop_back();
        rem_suf(root, static_cast<i32>(s.size()));
        tag.pop_back();
    }

    i32 prod(i32 i, i32 j) const {
        if (i == j) return static_cast<i32>(s.size()) - i;

        const i64 a = tag[mirror(i)];
        const i64 b = tag[mirror(j)];
        return prod(root, 0, inf<i64>, std::min(a, b) + 1, std::max(a, b));
    }

    i32 isa(i32 i) const {
        i = mirror(i);
        node *x = root;

        i32 ret = 0;
        while (x) {
            if (tag[x->sa] < tag[i]) {
                ret += size(x->l) + 1;
                x = x->r;
            } else {
                x = x->l;
            }
        }

        return ret;
    }

    i32 sa(i32 i) const {
        node *x = root;
        
        for (;;) {
            if (i < size(x->l)) {
                x = x->l;
            } else {
                i -= size(x->l);
                if (i == 0) return mirror(x->sa);

                i--;
                x = x->r;
            }
        }

        assert(0);
    }

    i32 lcp(i32 i) const {
        node *x = root;
        
        for (;;) {
            if (i < size(x->l)) {
                x = x->l;
            } else {
                i -= size(x->l);
                if (i == 0) return x->lcp;

                i--;
                x = x->r;
            }
        }

        assert(0);
    }

};

#endif // LIB_DYNAMIC_SUFFIX_ARRAY_HPP
