#ifndef LIB_DEGREE_SEQUENCE_HPP
#define LIB_DEGREE_SEQUENCE_HPP 1

#include <vector>
#include <numeric>
#include <cassert>
#include <algorithm>

#include <lib/prelude.hpp>

// http://compalg.inf.elte.hu/~tony/Kutatas/EGHH/Comb-IvanyiLucz-23Nov.pdf
inline bool erdos_gallai(const std::vector<i32> &d) {
        std::vector<i32> deg(d.begin(), d.end());
        const i32 n = static_cast<i32>(deg.size());

        if (n == 0) return true;
        if (*std::max_element(deg.begin(), deg.end()) >= n) return false;
        if (std::reduce(deg.begin(), deg.end()) % 2) return false;

        std::vector<i32> cnt(n);
        for (i32 i = 0; i < n; ++i) ++cnt[deg[i]];

        i32 p = n;
        for (i32 i = 0; i < n; ++i) {
                for (i32 j = 0; j < cnt[i]; ++j) deg[--p] = i;
        }

        std::vector<i64> h(n);
        std::inclusive_scan(deg.begin(), deg.end(), h.begin());

        i32 w = n - 1;
        for (i32 i = 0; i < n; ++i) {
                while (w >= 0 && deg[w] < i) --w;

                const i64 y = std::max(i, w);
                if (h[i] > y * (i + 1) + h[n - 1] - h[y]) return false;
        }

        return true;
}

inline std::vector<std::pair<i32, i32>> havel_hakimi(const std::vector<i32> &dg) {
        std::vector<i32> deg(dg.begin(), dg.end());
        const i32 n = static_cast<i32>(deg.size());

        std::vector<std::vector<i32>> d(n);
        for (i32 u = 0; u < n; ++u) d[deg[u]].push_back(u);

        std::vector<std::pair<i32, i32>> es;
        es.reserve(std::reduce(deg.begin(), deg.end()));

        i32 mx = n - 1;
        for (i32 i = 0; i < n; ++i) {
                while (mx >= 0 && d[mx].empty()) --mx;

                const i32 u = d[mx].back();
                d[mx].pop_back();

                std::vector<i32> nbd;

                i32 k = mx;
                while (static_cast<i32>(nbd.size()) < deg[u]) {
                        if (k == 0) return {};
                        if (d[k].empty()) {
                                --k;
                                continue;
                        }

                        const i32 v = d[k].back();
                        d[k].pop_back();

                        nbd.push_back(v);
                }

                for (const i32 v : nbd) {
                        es.emplace_back(u, v);

                        --deg[v];
                        d[deg[v]].push_back(v);
                }

                deg[u] = 0;
        }

        return es;
}

#endif // LIB_DEGREE_SEQUENCE_HPP