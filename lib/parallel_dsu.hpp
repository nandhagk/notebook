#ifndef LIB_PARALLEL_DSU_HPP
#define LIB_PARALLEL_DSU_HPP 1

#include <vector>

#include <lib/prelude.hpp>
#include <lib/dsu.hpp>

// same(L1,R1,L2,R2) みたいなことは出来ないと思う(必要なら rolling hash かな)
struct ParallelDSU {
	i32 N;
	i32 log;
	i32 ccs;
	// ufs[i][a]==ufs[i][b] iff [a,...,a+2^i) == [b,...,b+2^i)
	std::vector<DSU> ufs;
	ParallelDSU(i32 m) : N(m), ccs(m) {
		log = 1;
		while ((1 << log) < m) ++log;

		ufs.resize(log);
		for (i32 i = 0; i < log; ++i) {
			i32 n = 1 << i;
			ufs[i].Build(N - n + 1);
		}
	}

	i32 operator[](i32 i) { 
		return ufs[0][i]; 
	}

	// f(r1,r2) : 成分 r2 を r1 にマージ
	template <typename F>
	void Merge(i32 L1, i32 R1, i32 L2, i32 R2, F f) {
		assert(R1 - L1 == R2 - L2);

		i32 n = R1 - L1;
		if (n == 0) return;
		if (n == 1) return MergeInner(0, L1, L2, f);
		i32 k = topbit(n - 1);

		MergeInner(k, L1, L2, f);
		MergeInner(k, R1 - (1 << k), R2 - (1 << k), f);
	}

	// f(r1,r2) : 成分 r2 を r1 にマージ
	template <typename F>
	void Merge(i32 i, i32 j, F f) {
		MergeInner(0, i, j, f);
	}

	void Merge(i32 L1, i32 R1, i32 L2, i32 R2) {
		Merge(L1, R1, L2, R2, [&](i32, i32) -> void {});
	}

	void Merge(i32 i, i32 j) {
		Merge(i, j, [&](i32, i32) -> void {});
	}

	template <typename F>
	void MergeInner(i32 k, i32 L1, i32 L2, const F& f) {
		if (k == 0) {
			i32 a = ufs[0][L1], b = ufs[0][L2];
			if (a == b) return;
			ufs[0].Merge(a, b);
			i32 c = ufs[0][a];
			--ccs;
			return f(c, a ^ b ^ c);
		}

		if (!ufs[k].Merge(L1, L2)) return;
		MergeInner(k - 1, L1, L2, f);
		MergeInner(k - 1, L1 + (1 << (k - 1)), L2 + (1 << (k - 1)), f);
	}
};


#endif // LIB_PARALLEL_DSU_HPP 1
