#ifndef LIB_DENSE_GRAPH_HPP
#define LIB_DENSE_GRAPH_HPP 1

#include <vector>
#include <tr2/dynamic_bitset>

#include <lib/prelude.hpp>

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

                explicit edge_range(const std::tr2::dynamic_bitset<> &es):
                        e{es} {}

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

        edge_range operator[](i32 u) const {
                return edge_range(adj[u]);
        }

        dense_graph reverse() const {
                dense_graph h(n);

                for (i32 u = 0; u < n; ++u) {
                        for (const i32 v : operator[](u)) h.add_edge(v, u);
                }

                return h;
        }

        i32 n, m;
        std::vector<i32> deg, indeg, outdeg;
        std::vector<std::tr2::dynamic_bitset<>> adj;
};

#endif // LIB_DENSE_GRAPH_HPP
