#ifndef LIB_HOPCRAFT_KARP_HPP
#define LIB_HOPCRAFT_KARP_HPP 1

#include <utility>
#include <vector>
#include <random>
#include <algorithm>
#include <lib/prelude.hpp>

struct hopcroft_karp {
	std::vector<i32> g, l, r;
	i32 ans;

	hopcroft_karp(i32 n, i32 m, std::vector<std::pair<i32, i32>> &e):
		g(e.size()), l(n, -1), r(m, -1), ans(0) {
		std::random_device rd;
		std::mt19937 mt(rd());

		std::shuffle(e.begin(), e.end(), mt);
		std::vector<i32> deg(n + 1);

		for (const auto &[x, y] : e) deg[x]++;
		for (i32 i = 1; i <= n; i++) deg[i] += deg[i - 1];
		for (const auto &[x, y] : e) g[--deg[x]] = y;

		std::vector<i32> a, p, q(n);
		for (;;) {
			a.assign(n, -1), p.assign(n, -1);

			i32 t = 0;
			for (i32 i = 0; i < n; i++) {
				if (l[i] == -1) q[t++] = a[i] = p[i] = i;
			}

			bool match = false;
			for (i32 i = 0; i < t; i++) {
				i32 x = q[i];
				if (~l[a[x]]) continue;

				for (i32 j = deg[x]; j < deg[x + 1]; j++) {
					i32 y = g[j];
					if (r[y] == -1) {
						while (~y) {
							r[y] = x;
							std::swap(l[x], y);
							x = p[x];
						}

						match = true;
						++ans;

						break;
					}

					if (p[r[y]] == -1) {
						q[t++] = y = r[y];
						p[y] = x;
						a[y] = a[x];
					}
				}
			}

			if (!match) break;
		}
	}
};

#endif // LIB_HOPCRAFT_KARP_HPP 
