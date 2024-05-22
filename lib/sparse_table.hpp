#ifndef LIB_SPARSE_TABLE_HPP
#define LIB_SPARSE_TABLE_HPP 1

#include <algorithm>
#include <vector>
#include <functional>
#include <cassert>

namespace lib {

template<class T, auto op>
concept IsOp = std::is_convertible<decltype(op), std::function<T(T, T)>>::value;

template <class T, auto op>
	requires IsOp<T, op>
struct sparse_table {
public:
	explicit sparse_table(const std::vector<T> &v): n{static_cast<int>(v.size())} {
		log = std::bit_width(static_cast<unsigned int>(n));
		d = std::vector(log, std::vector<T>(n));

		std::copy(v.begin(), v.end(), d[0].begin());
		for (auto i = 1; i < log; ++i) {
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

} // namespace lib

#endif // LIB_SPARSE_TABLE_HPP
