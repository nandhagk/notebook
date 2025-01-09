#ifndef LIB_BECC_INCREMENTAL_HPP
#define LIB_BECC_INCREMENTAL_HPP 1

#include <vector>

#include <lib/dsu.hpp>
#include <lib/pbds.hpp>
#include <lib/prelude.hpp>

struct becc_incremental {
    i32 b;

private:
    dsu cc, bec;
    std::vector<i32> p;

    i32 parent(i32 u) { return p[u] = bec.find(p[u]); }

    void evert(i32 u) {
        std::vector<i32> V;

        for (;;) {
            V.push_back(u);
            if (bec.same(u, p[u])) break;
            u = parent(u);
        }

        while (!V.empty()) {
            u = V.back();
            V.pop_back();
            p[u] = (V.empty() ? u : V.back());
        }
    }

    void merge_cc(i32 u, i32 v) {
        if (cc.size(u) > cc.size(v)) std::swap(u, v);

        evert(u);
        p[u] = v;
        cc.merge(u, v);

        ++b;
    }

    i32 lca(i32 u, i32 v) {
        hash_set<i32> V;
        for (;;) {
            if (u != -1) {
                if (!V.insert(u).second) return u;

                i32 par = parent(u);
                u = (bec.same(u, par) ? -1 : par);
            }

            std::swap(u, v);
        }
    }

    i32 merge_becc(i32 u, i32 v) {
        i32 erased_bridge = 0;

        const i32 l = lca(u, v);
        while (!bec.same(u, l)) {
            i32 par = parent(u);
            bec.merge(u, l);
            u = par;
            ++erased_bridge;
        }

        while (!bec.same(v, l)) {
            i32 par = parent(v);
            bec.merge(v, l);
            v = par;
            ++erased_bridge;
        }

        p[bec.find(l)] = p[l];
        b -= erased_bridge;

        return erased_bridge;
    }

public:
    becc_incremental() {}

    explicit becc_incremental(i32 n) { build(n); }

    void build(i32 n) {
        b = 0;
        cc.build(n);
        bec.build(n);

        p.resize(n);
        std::iota(p.begin(), p.end(), 0);
    }

    bool merge(i32 u, i32 v) {
        assert(0 <= u && u < cc.n);
        assert(0 <= v && v < cc.n);

        u = bec.find(u);
        v = bec.find(v);

        if (u == v) return false;
        if (!cc.same(u, v)) {
            merge_cc(u, v);
            return false;
        }

        merge_becc(u, v);
        return true;
    }

    i32 size(i32 u) {
        assert(0 <= u && u < cc.n);

        return bec.size(u);
    }

    i32 find(i32 u) {
        assert(0 <= u && u < cc.n);

        return bec.find(u);
    }

    inline i32 ccs() const { return cc.ccs + b; }

    bool same(i32 u, i32 v) {
        assert(0 <= u && u < cc.n);
        assert(0 <= v && v < cc.n);

        return cc.same(u, v) && bec.same(u, v);
    }

    std::pair<i32, std::vector<i32>> ids() { return bec.ids(); }
};

#endif // LIB_BECC_INCREMENTAL_HPP
