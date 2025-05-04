#ifndef LIB_GRAPH_HPP
#define LIB_GRAPH_HPP 1

#include <type_traits>
#include <vector>

#include <lib/prelude.hpp>

struct simple_edge {
    using CostT = i32;

    i32 v, n;

    constexpr simple_edge()
        : simple_edge(0, 0) {}

    constexpr simple_edge(i32 to, i32 nxt)
        : v{to}, n{nxt} {}

    constexpr operator int() const {
        return v;
    }

    constexpr i32 data() const {
        return v;
    }
};

template <typename W>
struct weighted_edge {
    using CostT = W;

    i32 v, n;
    W w;

    constexpr weighted_edge()
        : weighted_edge(0, 0, W()) {}

    constexpr weighted_edge(i32 to, i32 nxt, W cost)
        : v{to}, n{nxt}, w{cost} {}

    constexpr operator int() const {
        return v;
    }

    constexpr std::pair<i32, W> data() const {
        return {v, w};
    }
};

template <typename E>
struct edge_range {
    const E *es;
    const i32 start;
    const usize sz;

    struct edge_iterator {
        const E *es;
        i32 i;

        explicit constexpr edge_iterator(const E *e, const i32 j)
            : es{e}, i{j} {}

        constexpr const edge_iterator &operator++() {
            i = es[i].n;
            return *this;
        }

        constexpr const edge_iterator operator++(int) {
            auto temp = *this;
            return operator++(), temp;
        }

        constexpr auto operator*() const {
            return es[i].data();
        }

        constexpr bool operator!=(const edge_iterator &it) const {
            return i != it.i;
        }

        constexpr bool operator==(const edge_iterator &it) const {
            return i == it.i;
        }
    };

    constexpr edge_iterator begin() {
        return edge_iterator(es, start);
    }

    constexpr edge_iterator end() {
        return edge_iterator(es, -1);
    }

    constexpr edge_iterator begin() const {
        return edge_iterator(es, start);
    }

    constexpr edge_iterator end() const {
        return edge_iterator(es, -1);
    }

    constexpr usize size() const {
        return sz;
    }
};

template <typename E>
constexpr bool is_simple_edge_v = std::is_same_v<E, simple_edge>;

template <typename E>
using is_simple_edge_t = std::enable_if_t<is_simple_edge_v<E>>;

template <typename E>
constexpr bool is_weighted_edge_v = std::is_same_v<E, weighted_edge<typename E::CostT>>;

template <typename E>
using is_weighted_edge_t = std::enable_if_t<is_weighted_edge_v<E>>;

template <typename E>
struct graph {
    using EdgeT = E;
    using CostT = typename EdgeT::CostT;

    explicit graph(i32 n)
        : graph(n, 0) {}

    graph(i32 n, i32 m)
        : head(n, -1), indeg(n), outdeg(n) {
        edges.reserve(m);
    }

    template <typename F = E, is_simple_edge_t<F> * = nullptr>
    void add_edge(i32 u, i32 v) {
        edges.emplace_back(v, head[u]);
        update(u, v);
    }

    template <typename F = E, is_weighted_edge_t<F> * = nullptr>
    void add_edge(i32 u, i32 v, CostT w) {
        edges.emplace_back(v, head[u], w);
        update(u, v);
    }

    edge_range<E> operator[](i32 u) {
        return {edges.data(), head[u], outdeg[u]};
    }

    edge_range<E> operator[](i32 u) const {
        return {edges.data(), head[u], outdeg[u]};
    }

    constexpr usize size() const {
        return head.size();
    }

    graph transpose() const {
        const i32 n = static_cast<i32>(size());

        graph h(n, static_cast<i32>(edges.size()));
        for (i32 u = 0; u < n; ++u) {
            for (i32 cur = head[u]; cur != -1;) {
                auto e = edges[cur];
                cur = e.n;

                const i32 v = e;
                e.v = u;
                e.n = h.head[v];

                h.edges.push_back(e);
                h.update(v, u);
            }
        }

        return h;
    }

    std::vector<i32> head;
    std::vector<usize> indeg, outdeg;
    std::vector<E> edges;

private:
    inline void update(i32 u, i32 v) {
        head[u] = static_cast<i32>(edges.size()) - 1;

        ++indeg[v];
        ++outdeg[u];
    }
};

#endif // LIB_GRAPH_HPP
