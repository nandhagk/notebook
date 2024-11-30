#ifndef LIB_GRAPH_HPP
#define LIB_GRAPH_HPP 1

#include <vector>
#include <numeric>
#include <tr2/dynamic_bitset>

#include <lib/prelude.hpp>

struct simple_edge {
        i32 v;

        simple_edge() {}
        simple_edge(i32 to):
                v{to} {}

        i32 to() const {
                return v;
        }

        operator int() const {
                return v;
        }
};

template <typename W>
struct weighted_edge {
        using CostT = W;

        i32 v;
        W w;

        weighted_edge() {}
        weighted_edge(i32 to, W cost):
                v{to}, w{cost} {}

        i32 to() const {
                return v;
        }

        operator int() const {
                return v;
        }
};

template <typename Edge>
struct graph {
        using E = Edge;

        graph() {}
        explicit graph(i32 p) {
                build(p);
        }

        void build(i32 p) {
                n = p;
                m = 0;

                deg.assign(n, 0);
                indeg.assign(n, 0);
                outdeg.assign(n, 0);

                adj.resize(n);
        }

        void add_edge(i32 u, const E& e) {
                adj[u].push_back(e);
                ++m;

                ++deg[u];
                ++deg[e.to()];

                ++indeg[e.to()];
                ++outdeg[u];
        }

        const std::vector<E> &operator[](i32 u) const {
                return adj[u];
        }

        i32 size() const {
                return n;
        }

        i32 n, m;
        std::vector<i32> deg, indeg, outdeg;
        std::vector<std::vector<E>> adj;
};

template <typename Edge>
struct csr_graph {
        using E = Edge;

        csr_graph() {}
        explicit csr_graph(i32 p, const std::vector<std::pair<i32, Edge>> &es) {
                build(p, es);
        }

        void build(i32 p, const std::vector<std::pair<i32, Edge>> &es) {
                n = p;
                m = static_cast<i32>(es.size());

                deg.assign(n, 0);
                indeg.assign(n, 0);
                outdeg.assign(n, 0);

                elist.resize(m);
                start.resize(n);

                for (const auto &[u, e] : es) {
                        ++deg[u];
                        ++deg[e.to()];
                        
                        ++indeg[e.to()];
                        ++outdeg[u];
                }

                std::inclusive_scan(outdeg.begin(), outdeg.end(), start.begin());
                for (const auto &[u, e] : es) elist[--start[u]] = e;
        }

        i32 size() const {
                return n;
        }

        struct edge_range {
                const E* es;
                usize size;

                const E* begin() {
                        return &es[0];
                }

                const E* end() {
                        return &es[size];
                }

                const E* begin() const {
                        return &es[0];
                }

                const E* end() const {
                        return &es[size];
                }
        };

        edge_range operator[](i32 u) const {
                return edge_range(elist.data() + start[u], outdeg[u]);
        }

        i32 n, m;
        std::vector<i32> deg, indeg, outdeg, start;
        std::vector<E> elist;
};

struct dense_graph {
        dense_graph() {}
        explicit dense_graph(i32 p) {
                build(p);
        }

        void build(i32 p) {
                n = p;
                m = 0;

                deg.assign(n, 0);
                indeg.assign(n, 0);
                outdeg.assign(n, 0);

                adj.assign(n, std::tr2::dynamic_bitset<>(n));
        }

        void add_edge(i32 u, i32 v) {
                adj[u].set(v);
                ++m;

                ++deg[u];
                ++deg[v];

                ++indeg[v];
                ++outdeg[u];
        }

        i32 size() const {
                return n;
        }

        struct edge_range {
                const std::tr2::dynamic_bitset<> &e;

                struct edge_iterator {
                        const std::tr2::dynamic_bitset<> &e;
                        usize v;

                        edge_iterator(const std::tr2::dynamic_bitset<> &es, usize s):
                                e{es}, v{s} {}
                        
                        const edge_iterator& operator++() {
                                v = e.find_next(v);
                                return *this;
                        }

                        const edge_iterator operator++(i32) {
                                const auto tmp = *this;
                                operator++();

                                return tmp;
                        }

                        i32 operator*() const {
                                return static_cast<i32>(v);
                        }

                        bool operator==(const edge_iterator& it) const {
                                return v == it.v;
                        }

                        bool operator!=(const edge_iterator& it) const {
                                return v != it.v;
                        }
                };

                edge_iterator begin() {
                        return edge_iterator(e, e.find_first());
                }

                edge_iterator end() {
                        return edge_iterator(e, e.size());
                }

                edge_iterator begin() const {
                        return edge_iterator(e, e.find_first());
                }

                edge_iterator end() const {
                        return edge_iterator(e, e.size());
                }
        };

        i32 n, m;
        std::vector<i32> deg, indeg, outdeg;
        std::vector<std::tr2::dynamic_bitset<>> adj;
};

#endif // LIB_GRAPH_HPP
