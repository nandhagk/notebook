#ifndef LIB_CCC_HPP
#define LIB_CCC_HPP 1

#include <numeric>
#include <queue>
#include <vector>

#include <lib/prelude.hpp>

template <typename UndirectedGraph>
inline std::pair<i32, std::vector<i32>> ccc(const UndirectedGraph &g) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<i32> ids(n, -1), rem(n), flg(n);
    std::iota(rem.begin(), rem.end(), 0);

    i32 group{};
    while (!rem.empty()) {
        const i32 r = rem.back();
        rem.pop_back();

        std::queue<i32> q;
        ids[r] = group;

        q.push(r);
        while (!q.empty()) {
            const i32 u = q.front();
            q.pop();

            for (const i32 v : g[u]) flg[v] = 1;

            std::vector<i32> nxtrem;
            for (const i32 v : rem) {
                if (flg[v]) {
                    nxtrem.push_back(v);
                } else if (ids[v] == -1) {
                    ids[v] = group;
                    q.push(v);
                }
            }

            for (const i32 v : g[u]) flg[v] = 0;
            rem = nxtrem;
        }

        ++group;
    }

    return {group, std::move(ids)};
}

#endif // LIB_CCC_HPP
