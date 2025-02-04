#ifndef LIB_DYNAMIC_CONNECTIVITY_OFFLINE_HPP
#define LIB_DYNAMIC_CONNECTIVITY_OFFLINE_HPP 1

#include <cassert>

#include <lib/pbds.hpp>
#include <lib/prelude.hpp>

template <typename RollbackDSU, typename F = std::function<void(RollbackDSU &, i32)>>
struct dynamic_connectivity_offline {
private:
    i32 n;
    hash_map<i64, i32> e;
    std::vector<std::pair<i32, F>> q;

    struct update {
        i32 a, b, link_time, cut_time;
    };

    std::vector<update> u{update{0, 0, 0, 0}};

    void solve(i32 l, i32 r, i32 &j, i32 &k) {
        assert(l != r);

        if (l + 1 == r) {
            while (k < static_cast<i32>(q.size()) && q[k].first == l) q[k++].second(dsu, j++);
            return;
        }

        const i32 mid = (r + l) / 2;

        i32 cnt{};
        for (i32 i = mid; i < r; ++i)
            if (u[i].link_time <= l && dsu.merge(u[i].a, u[i].b)) ++cnt;

        solve(l, mid, j, k);
        dsu.rollback(cnt);

        cnt = 0;
        for (i32 i = l; i <= mid; ++i)
            if (r <= u[i].cut_time && dsu.merge(u[i].a, u[i].b)) ++cnt;

        solve(mid, r, j, k);
        dsu.rollback(cnt);
    }

public:
    RollbackDSU dsu;

    dynamic_connectivity_offline(i32 m)
        : n{m}, dsu(n) {}

    void link(i32 a, i32 b) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        if (a > b) std::swap(a, b);
        if (a == b) return;

        const i32 link_time = static_cast<i32>(u.size());

        e[(static_cast<i64>(a) << 30) | b] = link_time;
        u.push_back(update{a, b, link_time, inf<i32>});
    }

    void cut(i32 a, i32 b) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        if (a > b) std::swap(a, b);
        if (a == b) return;

        const i32 link_time = e[(static_cast<i64>(a) << 30) | b];
        const i32 cut_time = static_cast<i32>(u.size());

        u[link_time].cut_time = cut_time;
        u.push_back(update{a, b, link_time, cut_time});
    }

    void query(F f) {
        const i32 query_time = static_cast<i32>(u.size()) - 1;
        q.emplace_back(query_time, f);
    }

    void solve() {
        i32 j{}, k{};
        solve(0, static_cast<i32>(u.size()), j, k);
    }
};

#endif // LIB_DYNAMIC_CONNECTIVITY_OFFLINE_HPP
