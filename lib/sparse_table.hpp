#ifndef LIB_SPARSE_TABLE_HPP
#define LIB_SPARSE_TABLE_HPP 1

#include <vector>
#include <functional>
#include <cassert>

template<class T, auto op>
concept IsSparseTableOp = std::is_convertible<decltype(op), std::function<T(T, T)>>::value;

template <class T, auto op>
	requires IsSparseTableOp<T, op>
struct SparseTable {
public:
	explicit SparseTable(const std::vector<T> &v):
		n(static_cast<int>(v.size())),
		log(std::bit_width(static_cast<unsigned int>(n))),
		d(1, v)
	{
		for (auto i = 1; i < log; ++i) {
			d.emplace_back(n - (1 << i) + 1);
			for (auto j = 0; j + (1 << i) <= n; ++j) {
				d[i][j] = op(d[i - 1][j], d[i - 1][j + (1 << (i - 1))]);
			}
		}
	}

	T prod(int l, int r) const {
		assert(0 <= l && l < r && r <= n);

		const auto p = std::bit_width(static_cast<unsigned int>(r - l)) - 1;
		return op(d[p][l], d[p][r - (1 << p)]);
	}

private:
	int n, log;
	std::vector<std::vector<T>> d;
};

#endif // LIB_SPARSE_TABLE_HPP
