#ifndef LIB_INDEPENDENT_SET_HPP
#define LIB_INDEPENDENT_SET_HPP 1

#include <tr2/dynamic_bitset>
#include <vector>

#include <lib/prelude.hpp>

template <typename UndirectedGraph>
std::vector<i32> independent_set(const UndirectedGraph &g) {
    using bs = std::tr2::dynamic_bitset<>;

    const i32 n = static_cast<i32>(g.size());
    std::vector<bs> nbd(n, bs(n));

    for (i32 u = 0; u < n; ++u)
        for (const i32 v : g[u]) nbd[u][v] = 1;

    i32 best = 0;
    bs res(n);

    const auto dfs = [&](auto &&self, bs now, bs rest) -> void {
        i32 v = -1, e = -1;
        for (;;) {
            bool upd = false;
            for (i32 u = 0; u < n; ++u) {
                if (!rest[u]) continue;

                const i32 d = static_cast<i32>((nbd[u] & rest).count());
                if (e < d) {
                    v = u;
                    e = d;
                }

                if (d <= 1) {
                    rest[u] = 0;
                    rest &= ~nbd[u];
                    now[u] = 1;
                    upd = true;
                }
            }

            if (!upd) break;

            v = -1;
            e = -1;
        }

        const i32 a = static_cast<i32>(now.count());
        const i32 b = static_cast<i32>(rest.count());

        if (best < a) {
            best = a;
            res = now;
        }

        if (b == 0 || a + b <= best) return;

        rest[v] = 0;

        if (e >= 3) self(self, now, rest);
        now[v] = 1;

        self(self, now, rest & (~nbd[v]));
    };

    bs now(n), rest(n);

    for (i32 u = 0; u < n; ++u) rest[u] = 1;
    dfs(dfs, now, rest);

    std::vector<i32> out;
    out.reserve(n);

    for (i32 u = 0; u < n; ++u)
        if (res[u]) out.push_back(u);

    return out;
};

#endif // LIB_INDEPENDENT_SET_HPP
