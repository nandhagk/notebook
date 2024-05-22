#ifndef LIB_SEGTREE_HPP
#define LIB_SEGTREE_HPP 1

#include <algorithm>
#include <vector>
#include <functional>
#include <cassert>

namespace lib {

template<class S, auto op>
concept IsOp = std::is_convertible<decltype(op), std::function<S(S, S)>>::value;

template <class S, auto e>
concept IsE = std::is_convertible<decltype(e), std::function<S()>>::value;

template <class S, auto e, class F>
concept IsF = IsE<S, e> && std::is_convertible<F, std::function<bool(S)>>::value;

template <class S, auto op, auto e>
	requires IsOp<S, op> && IsE<S, e>
struct segtree {
public:
	explicit segtree(const int n_): segtree(std::vector<S>(n_, e())) {}
	explicit segtree(const std::vector<S> &v): n{static_cast<int>(v.size())} {
		size = std::bit_ceil(static_cast<unsigned int>(n));
		log = std::countr_zero(static_cast<unsigned int>(size));
		d = std::vector<S>(2 * size, e());

		std::copy(v.begin(), v.end(), d.begin() + size);
		for (auto i = size - 1; i >= 1; --i) update(i);
	}

	void set(int p, S x) {
		assert(0 <= p && p < n);

		p += size;
		d[p] = x;

		for (auto i = 1; i <= log; i++) update(p >> i);
	}

	S get(int p) const {
		assert(0 <= p && p < n);

		return d[p + size];
	}

	S prod(int l, int r) const {
		assert(0 <= l && l < r && r <= n);

		S sml = e(), smr = e();
		l += size;
		r += size;

		while (l < r) {
			if (l & 1) sml = op(sml, d[l++]);
			if (r & 1) smr = op(d[--r], smr);
			l >>= 1;
			r >>= 1;
		}

		return op(sml, smr);
	}

	S all_prod() const { return d[1]; }

	template <class F> requires IsF<S, e, F>
	int max_right(int l, F f) const {
		assert(0 <= l && l <= n);
		assert(f(e()));

		if (l == n) return n;

		l += size;
		S sm = e();

		do {
			while (l % 2 == 0) l >>= 1;
			if (!f(op(sm, d[l]))) {
				while (l < size) {
					l = 2 * l;
					if (f(op(sm, d[l]))) {
						sm = op(sm, d[l]);
						++l;
					}
				}

				return l - size;
			}

			sm = op(sm, d[l]);
			++l;
		} while ((l & -l) != l);

		return n;
	}

	template <class F> requires IsF<S, e, F>
	int min_left(int r, F f) const {
		assert(0 <= r && r <= n);
		assert(f(e()));

		if (r == 0) return 0;

		r += size;
		S sm = e();

		do {
			--r;
			while (r > 1 && (r % 2)) r >>= 1;

			if (!f(op(d[r], sm))) {
				while (r < size) {
					r = 2 * r + 1;
					if (f(op(d[r], sm))) {
						sm = op(d[r], sm);
						--r;
					}
				}

				return r + 1 - size;
			}

			sm = op(d[r], sm);
		} while ((r & -r) != r);

		return 0;
	}

private:
	int n, size, log;
	std::vector<S> d;

	void update(int k) { d[k] = op(d[2 * k], d[2 * k + 1]); }
};

} // namespace lib

#endif // LIB_SEGTREE_HPP
