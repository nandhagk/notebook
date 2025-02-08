#ifndef LIB_BINARY_TRIE_HPP
#define LIB_BINARY_TRIE_HPP 1

#include <array>
#include <limits>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename U, i32 B = std::numeric_limits<U>::digits>
struct binary_trie {
    static constexpr i32 W = std::numeric_limits<U>::digits;

    struct node {
        U k;
        i32 l, r, count;
        std::array<i32, 2> ch{};

        node(U _k = 0, i32 _l = 0, i32 _r = 0)
            : k(_k), l(_l), r(_r), count(0) {
            ch[0] = ch[1] = -1;
        }
    };

    i32 root, sz;
    std::vector<node> nodes;

    static constexpr U mask(i32 l, i32 r) {
        return static_cast<u32>(((static_cast<u64>(1) << (r - l + 1)) - 1) << l);
    }

    binary_trie() {}
    explicit binary_trie(i32 n) {
        build(n);
    }

    void build(i32 n) {
        root = 0;
        sz = 0;

        nodes = {node()};
        nodes.reserve(n + 1);
    }

    i32 make_node(U x, i32 l, i32 r) {
        nodes.emplace_back(x, l, r);
        return ++sz;
    }

    i32 next_bit(i32 v, U k) const {
        U x = (nodes[v].k ^ k) & mask(nodes[v].l, nodes[v].r);
        return x ? topbit(x) : -1;
    }

    void insert(U x, i32 k = 1) {
        i32 v = root;
        i32 bit = B - 1;
        while (bit >= 0) {
            nodes[v].count += k;
            i32 nxt = (x >> bit) & 1;
            if (nodes[v].ch[nxt] == -1) {
                nodes[v].ch[nxt] = make_node(x, 0, bit);
                nodes[nodes[v].ch[nxt]].count = k;
                return;
            }
            v = nodes[v].ch[nxt];
            i32 diff_bit = std::max(nodes[v].l - 1, next_bit(v, x));
            if (diff_bit == nodes[v].l - 1) {
                bit = diff_bit;
            } else {
                nxt = (x >> diff_bit) & 1;
                i32 tmp = make_node(nodes[v].k, nodes[v].l, diff_bit);
                nodes[v].l = diff_bit + 1;
                nodes[tmp].count = nodes[v].count;
                nodes[v].count += k;
                nodes[tmp].ch[!nxt] = nodes[v].ch[!nxt];
                nodes[tmp].ch[nxt] = nodes[v].ch[nxt];
                nodes[v].ch[!nxt] = tmp;
                nodes[v].ch[nxt] = make_node(x, 0, diff_bit);
                nodes[nodes[v].ch[nxt]].count = k;
                return;
            }
        }
        nodes[v].count += k;
    }

    i32 find(U x, i32 v, i32 bit) const {
        v = nodes[v].ch[(x >> bit) & 1];
        if (!v) return 0;
        i32 diff_bit = std::max(nodes[v].l - 1, next_bit(v, x));
        if (diff_bit != nodes[v].l - 1) return 0;
        if (nodes[v].l == 0) return nodes[v].count;
        return find(x, v, diff_bit);
    }

    i32 count(i32 v) const {
        return (v == -1 ? 0 : nodes[v].count);
    }

    i32 erase(U x, i32 k, i32 v, i32 bit) {
        v = nodes[v].ch[(x >> bit) & 1];
        if (count(v) == 0) return 0;
        i32 diff_bit = std::max(nodes[v].l - 1, next_bit(v, x));
        if (diff_bit > nodes[v].l - 1) return 0;
        if (nodes[v].l == 0) {
            k = std::min(k, nodes[v].count);
            nodes[v].count -= k;
            return k;
        }
        k = erase(x, k, v, diff_bit);
        nodes[v].count -= k;
        return k;
    }

    i32 erase(U x, i32 k = std::numeric_limits<int>::max()) {
        k = erase(x, k, root, B - 1);
        nodes[root].count -= k;
        return k;
    }

    i32 find(U x) const {
        return find(x, root, B - 1);
    }

    i32 size() const {
        return nodes[root].count;
    }

    U xor_min(U x) const {
        assert(size());
        i32 v = root;
        i32 bit = B - 1;
        U k = 0;
        while (bit >= 0) {
            i32 target = (x >> bit) & 1;
            if (count(nodes[v].ch[target]) == 0) target ^= 1;
            v = nodes[v].ch[target];
            k += (x ^ nodes[v].k) & mask(nodes[v].l, nodes[v].r);
            bit = nodes[v].l - 1;
        }
        return k;
    }

    U xor_max(U x) const {
        assert(size());
        i32 v = root;
        i32 bit = B - 1;
        U k = 0;
        while (bit >= 0) {
            i32 target = ((x >> bit) & 1) ^ 1;
            if (count(nodes[v].ch[target]) == 0) target ^= 1;
            v = nodes[v].ch[target];
            k += (x ^ nodes[v].k) & mask(nodes[v].l, nodes[v].r);
            bit = nodes[v].l - 1;
        }
        return k;
    }

    i32 next(i32 v, i32 k, bool is_right) const {
        if (v == root || k == nodes[v].l - 1)
            return nodes[v].ch[is_right];
        else if (((nodes[v].k >> k) & 1) == is_right)
            return v;
        else
            return -1;
    }

    // is there w in the trie such that l <= x ^ w < r
    bool xor_range_satisfiable(U x, U l, U r) {
        if (l >= r) return false;
        r--;
        i32 v = root;
        for (i32 i = B - 1; i >= 0; i--) {
            if (((l >> i) & 1) == ((r >> i) & 1)) {
                i32 target = ((x >> i) & 1) ^ ((l >> i) & 1);
                v = next(v, i, target);
                if (count(v) == 0) return false;
            } else {
                i32 ith = (x >> i) & 1;
                i32 u = next(v, i, ith);
                if (u != -1) {
                    i32 j = i - 1;
                    for (; j >= 0 && u != -1; j--) {
                        i32 target = (l >> j) & 1, flip = (x >> j) & 1;
                        i32 ch_sz = count(next(u, j, flip ^ 1));
                        if (target == 1 && ch_sz == 0) break;
                        if (target == 0 && ch_sz) return true;
                        u = next(u, j, target ^ flip);
                    }
                    if (j == -1 && count(u)) return true;
                }
                u = next(v, i, ith ^ 1);
                if (u != -1) {
                    i32 j = i - 1;
                    for (; j >= 0 && u != -1; j--) {
                        i32 target = (r >> j) & 1, flip = (x >> j) & 1;
                        i32 ch_sz = count(next(u, j, flip));
                        if (target == 0 && ch_sz == 0) break;
                        if (target == 1 && ch_sz) return true;
                        u = next(u, j, target ^ flip);
                    }
                    if (j == -1 && count(u)) return true;
                }
                return false;
            }
        }
        return true;
    }
};

#endif // LIB_BINARY_TRIE_HPP
