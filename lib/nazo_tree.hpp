#ifndef LIB_NAZO_TREE_HPP
#define LIB_NAZO_TREE_HPP 1

#include <algorithm>
#include <array>
#include <optional>
#include <string>

#include <lib/prelude.hpp>

template <i32 D>
struct nazo_tree {
private:
    static_assert(1 <= D && D <= 4); // 64, 4096, 262144, 16777216

    static constexpr i32 len_block = 64;
    static constexpr i32 node_count = (1 << (6 * D)) / (len_block - 1); // 1 + 64 + 64^2 ... 64^(D-1)
    static constexpr i32 inf = 1 << (6 * D);

    std::array<u64, node_count> flag;

public:
    nazo_tree() {}
    explicit nazo_tree(const std::string &s, char one = '1') {
        build(s, one);
    }

    void build(const std::string &s, char one = '1') {
        flag.fill(0);
        i32 n = std::min((i32)s.size(), 1 << (6 * D));
        for (i32 i = 0; i < n; i++) {
            if (s[i] == one) {
                i32 j = (i + node_count - 1);
                flag[j / len_block] |= static_cast<u64>(1) << (j % len_block);
            }
        }
        for (i32 i = node_count - 1; i > 0; i--)
            if (flag[i]) flag[(i - 1) / len_block] |= static_cast<u64>(1) << ((i - 1) % len_block);
    }

    void insert(i32 k) {
        k += node_count;
        while (k--) {
            i32 dir = k % len_block;
            k /= len_block;
            flag[k] |= static_cast<u64>(1) << dir;
        }
    }

    void erase(i32 k) {
        k += node_count;
        bool f = true;
        while (k--) {
            i32 dir = k % len_block;
            k /= len_block;
            flag[k] &= ~((u64)f << dir);
            f = !flag[k];
        }
    }

    bool find(i32 k) const {
        k += node_count;
        i32 p0 = (k - 1) / len_block;
        i32 d0 = (k - 1) % len_block;
        return (flag[p0] >> d0) & 1;
    }

    std::optional<i32> find_next(i32 k) const {
        if (k >= inf) return std::nullopt;

        k = std::max(k, 0);
        if (find(k)) return k;

        k += node_count;
        while (k--) {
            const i32 dir = k % len_block;
            k /= len_block;

            if (const u64 f = (flag[k] >> dir) >> 1; f) {
                k = (k * len_block) + dir + 2 + __builtin_ctzll(f);
                while (k < node_count) k = (k * len_block) + __builtin_ctzll(flag[k]) + 1;
                return k - node_count;
            }
        }

        return std::nullopt;
    }

    std::optional<i32> find_prev(i32 k) const {
        if (k < 0) return std::nullopt;

        k = std::min(k, inf - 1);
        if (find(k)) return k;

        k += node_count;
        while (k--) {
            const i32 dir = k % len_block;
            k /= len_block;

            if (const u64 f = flag[k] & ((static_cast<u64>(1) << dir) - 1); f) {
                k = (k * len_block) + len_block - __builtin_clzll(f);
                while (k < node_count) k = (k * len_block) + len_block - __builtin_clzll(flag[k]);
                return k - node_count;
            }
        }

        return std::nullopt;
    }
};

#endif // LIB_NAZO_TREE_HPP
