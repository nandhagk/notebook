#ifndef LIB_TWO_SAT_HPP
#define LIB_TWO_SAT_HPP 1

#include <vector>
#include <cassert>
#include <optional>

#include <lib/prelude.hpp>
#include <lib/csr_graph.hpp>
#include <lib/scc.hpp>

struct two_sat {
	i32 n;
	std::vector<std::pair<i32, simple_edge>> es;

	two_sat() {}
	explicit two_sat(i32 m) {
		build(m);
	}

	void build(i32 m) {
		n = m;
	}

	void add_clause(i32 i, bool p, i32 j, bool q) {
		assert(0 <= i && i < n);
		assert(0 <= j && j < n);

		es.emplace_back(2 * i + (p ? 0 : 1), 2 * j + (q ? 1 : 0));
		es.emplace_back(2 * j + (q ? 0 : 1), 2 * i + (p ? 1 : 0));
	}

	std::optional<std::vector<bool>> satisfiable() {
		std::vector<bool> ans(n, false);

		csr_graph<simple_edge> g(2 * n, es);
		const auto &[_, ids] = scc(g);

		for (i32 i = 0; i < n; ++i) {
			if (ids[2 * i] == ids[2 * i + 1]) return std::nullopt;
			ans[i] = ids[2 * i] < ids[2 * i + 1];
		}

		return ans;
	}
};

#endif //  LIB_TWO_SAT_HPP
