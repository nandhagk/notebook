#ifndef LIB_HUNGARIAN_HPP
#define LIB_HUNGARIAN_HPP 1

#include <vector>
#include <numeric>
#include <cassert>
#include <lib/prelude.hpp>

template <typename T>
std::pair<T, std::vector<i32>> hungarian(const std::vector<std::vector<T>> &cost) {
	const i32 n = static_cast<i32>(cost.size());

	std::vector<i32> row_mate(n, -1), col_mate(n, -1);
	std::vector<T> pi(n, 0);

	const auto residual = [&](i32 r, i32 c) { return cost[r][c] - pi[c]; };

	// column reduction, mate columns greedily
	std::vector<bool> transferrable(n, false);
	for (i32 col = 0; col < n; col++) {
		i32 row = 0;
		for (i32 u = 1; u < n; u++) {
			if (cost[row][col] > cost[u][col]) { row = u; }
		}
		pi[col] = cost[row][col];
		if (row_mate[row] == -1) {
			row_mate[row] = col;
			col_mate[col] = row;
			transferrable[row] = true;
		} else {
			transferrable[row] = false;
		}
	}

	std::vector<i32> cols(n);
	std::iota(cols.begin(), cols.end(), 0);

	for (i32 row = 0; row < n; row++) {
		if (row_mate[row] != -1) { continue; }
		std::vector<T> dist(n);
		for (i32 c = 0; c < n; c++) { dist[c] = residual(row, c); }
		std::vector<i32> pred(n, row);

		i32 scanned = 0, labeled = 0, last = 0;
		i32 col = -1;

		while (true) {
			if (scanned == labeled) {
				last = scanned;
				T min = dist[cols[scanned]];
				for (i32 j = scanned; j < n; j++) {
					i32 c = cols[j];
					if (dist[c] <= min) {
						if (dist[c] < min) {
							min = dist[c];
							labeled = scanned;
						}
						std::swap(cols[j], cols[labeled++]);
					}
				}
				for (i32 j = scanned; j < labeled; j++) {
					if (i32 c = cols[j]; col_mate[c] == -1) {
						col = c;
						goto done;
					}
				}
			}

			assert(scanned < labeled);
			i32 c1 = cols[scanned++];
			i32 r1 = col_mate[c1];

			for (i32 j = labeled; j < n; j++) {
				i32 c2 = cols[j];
				T len = residual(r1, c2) - residual(r1, c1);
				assert(len >= 0);

				if (dist[c2] > dist[c1] + len) {
					dist[c2] = dist[c1] + len;
					pred[c2] = r1;
					if (len == 0) {
						if (col_mate[c2] == -1) {
							col = c2;
							goto done;
						}

						std::swap(cols[j], cols[labeled++]);
					}
				}
			}
		}

	done:;
		for (i32 i = 0; i < last; i++) {
			i32 c = cols[i];
			pi[c] += dist[c] - dist[col];
		}

		i32 t = col;
		while (t != -1) {
			col = t;
			i32 r = pred[col];
			col_mate[col] = r;
			std::swap(row_mate[r], t);
		}
	}

	T total = 0;
	for (i32 u = 0; u < n; u++) { total += cost[u][row_mate[u]]; }

	return {total, row_mate};
}

#endif // LIB_HUNGARIAN_HPP
