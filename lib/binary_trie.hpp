#ifndef LIB_BINARY_TRIE_HPP
#define LIB_BINARY_TRIE_HPP 1

#include <array>
#include <limits>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename U, is_unsigned_integral_t<U> * = nullptr>
struct binary_trie {
    static constexpr i32 W = std::numeric_limits<U>::digits;

    struct node {
        std::array<i32, 2> to{};
        i32 count{};
    };

    std::vector<node> nodes;

    binary_trie() : nodes{node()} {}

    explicit binary_trie(i32 n) : binary_trie() {
        nodes.reserve(2 * n);
    }

    i32 go(i32 v, bool c) {
        if (!nodes[v].to[c]) {
            nodes[v].to[c] = static_cast<i32>(nodes.size());
            nodes.emplace_back();
        }

        return nodes[v].to[c];
    }

    std::array<i32, W> path(U x) {
        std::array<i32, W> r;

        for (i32 i = W - 1, v = 0; i >= 0; --i) {
            const bool c = (x >> i) & 1;

            v = go(v, c);
            r[i] = v;
        }

        return r;
    }

    void insert(U x) {
        const auto p = path(x);
        if (nodes[p[0]].count == 1) return;

        for (const i32 v : p) ++nodes[v].count;
    }

    void erase(U x) {
        const auto p = path(x);
        if (nodes[p[0]].count != 1) return;

        for (const i32 v : p) --nodes[v].count;
    }

    U min_xor(U x) const {
        U a = 0;
        for (i32 i = W - 1, v = 0; i >= 0; --i) {
            const bool c = (x >> i) & 1;

            if (nodes[nodes[v].to[c]].count) {
                v = nodes[v].to[c];
            } else {
                a ^= U(1) << i;
                v = nodes[v].to[!c];
            }
        }

        return a;
    }

    U max_xor(U x) const {
        return min_xor(~x);
    }
};

#endif // LIB_BINARY_TRIE_HPP
