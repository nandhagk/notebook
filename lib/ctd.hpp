#ifndef LIB_CENTROID_HPP
#define LIB_CENTROID_HPP 1

#include <vector>
#include <lib/prelude.hpp>

struct ctd {
	using Tree = std::vector<std::vector<i32>>;

	explicit ctd(const Tree& g) : ctd(g, 0) {}

	ctd(const Tree& g, i32 root) :
		n(static_cast<i32>(g.size())), g(g),
		par(n, -1), sz(n, 1), seen(n, false) {
		init_centroid(root);
	}

	i32 find_centroid(i32 u, i32 t, i32 k) {
                for (const i32 v : g[u]) {
                        if (seen[v] || v == t) continue;
                        if (sz[v] > k / 2) return find_centroid(v, u, k);
                }

                return u;
        };

        i32 find_size(i32 u, i32 t = -1) {
                if (seen[u]) return 0;

                sz[u] = 1;
                for (const i32 v : g[u]) {
                        if (v == t) continue;
                        sz[u] += find_size(v, u);
                }

                return sz[u];
        };

        void init_centroid(i32 u, i32 t = -1) {
                find_size(u);

                const i32 c = find_centroid(u, -1, sz[u]);
                seen[c] = true;
                par[c] = t;

                for (const i32 v : g[c]) {
                        if (seen[v]) continue;
                        init_centroid(v, c);
                }
        };

	i32 n;
        const Tree& g;
	std::vector<i32> par, sz;
	std::vector<bool> seen;
};

#endif // LIB_CENTROID_HPP
