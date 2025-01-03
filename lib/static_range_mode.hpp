#ifndef LIB_STATIC_RANGE_MODE_HPP
#define LIB_STATIC_RANGE_MODE_HPP 1

#include <algorithm>
#include <cmath>
#include <vector>

#include <lib/prelude.hpp>

template <typename T>
struct static_range_mode {
    i32 n, sq;
    std::vector<T> cmp;
    std::vector<i32> val, rank;
    std::vector<std::vector<i32>> pos;
    std::vector<std::vector<std::pair<i32, i32>>> large;

    static_range_mode() {}
    explicit static_range_mode(const std::vector<T> &v) { build(v); }

    void build(const std::vector<T> &v) {
        n = static_cast<i32>(v.size());
        sq = static_cast<i32>(std::sqrt(n));

        cmp.resize(n);
        val.resize(n);
        rank.resize(n);
        pos.resize(n);
        large.assign(n / sq + 3, std::vector<std::pair<i32, i32>>(n / sq + 3));

        for (i32 i = 0; i < n; i++) cmp[i] = v[i];

        std::sort(cmp.begin(), cmp.end());
        cmp.erase(std::unique(cmp.begin(), cmp.end()), cmp.end());

        for (i32 i = 0; i < n; i++) {
            const i32 id = static_cast<i32>(std::lower_bound(cmp.begin(), cmp.end(), v[i]) - cmp.begin());
            val[i] = id;
            rank[i] = static_cast<i32>(pos[id].size());
            pos[id].push_back(i);
        }

        for (i32 i = 0; i * sq < n; i++) {
            std::vector<i32> cnt(n, 0);
            i32 l = i * sq, r = std::min(n, l + sq);
            std::pair<i32, i32> m{0, 0};
            for (i32 j = l, k = i + 1; j < n; j++) {
                cnt[val[j]]++;
                m = std::max(m, {cnt[val[j]], val[j]});
                if (j + 1 == r) {
                    large[i][k++] = m;
                    r = std::min(n, r + sq);
                }
            }
        }
    }

    std::pair<T, i32> mode(i32 l, i32 r) const {
        const i32 L = (l + sq - 1) / sq, R = r / sq;
        std::pair<i32, i32> m;

        const auto left = [&](i32 _l, i32 _r) -> void {
            for (i32 i = _l; i < _r; i++) {
                const i32 x = val[i], k = rank[i];
                if (k + m.first >= static_cast<i32>(pos[x].size()) || pos[x][k + m.first] >= r) continue;
                i32 j = k + m.first;
                while (j < static_cast<i32>(pos[x].size()) && pos[x][j] < r) j++;
                m = std::max(m, {j - k, x});
            }
        };

        if (L >= R) {
            left(l, r);
        } else {
            m = large[L][R];
            left(l, L * sq);
            for (i32 i = r - 1; i >= R * sq; i--) {
                const i32 x = val[i], k = rank[i];
                if (k - m.first < 0 || pos[x][k - m.first] < l) continue;
                i32 j = k - m.first;
                while (j >= 0 && pos[x][j] >= l) j--;
                m = std::max(m, {k - j, x});
            }
        }

        return {cmp[m.second], m.first};
    }
};

#endif // LIB_STATIC_RANGE_MODE_HPP
