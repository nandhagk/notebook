#ifndef LIB_SEGMENT_TREE_HPP
#define LIB_SEGMENT_TREE_HPP 1

#include <vector>
#include <functional>
#include <cassert>
#include <lib/prelude.hpp>

template<class S, auto op>
concept IsSegmentTreeOp = std::is_convertible<decltype(op), std::function<S(S, S)>>::value;

template <class S, auto e>
concept IsSegmentTreeE = std::is_convertible<decltype(e), std::function<S()>>::value;

template <class S, class F>
concept IsSegmentTreeF = std::is_convertible<F, std::function<bool(S)>>::value;

template <class S, auto op, auto e>
	requires IsSegmentTreeOp<S, op> && IsSegmentTreeE<S, e>
struct SegmentTree {
public:
	explicit SegmentTree(const i32 n_): SegmentTree(std::vector<S>(n_, e())) {}

	explicit SegmentTree(const std::vector<S> &v):
		n(static_cast<i32>(v.size())),
		size(std::bit_ceil(static_cast<u32>(n))),
		log(std::countr_zero(static_cast<u32>(size))),
		d(2 * size, e())
	{
		for (i32 i = 0; i < n; i++) d[size + i] = v[i];
		for (i32 i = size - 1; i >= 1; --i) update(i);
	}

	void set(i32 p, S x) {
		assert(0 <= p && p < n);

		p += size;
		d[p] = x;

		for (i32 i = 1; i <= log; i++) update(p >> i);
	}

	S get(i32 p) const {
		assert(0 <= p && p < n);

		return d[p + size];
	}

	S prod(i32 l, i32 r) const {
		assert(0 <= l && l <= r && r <= n);

		S sml = e(), smr = e();
		for (l += size, r += size; l < r; l >>= 1, r >>= 1) {
			if (l & 1) sml = op(sml, d[l++]);
			if (r & 1) smr = op(d[--r], smr);
		}

		return op(sml, smr);
	}

	S all_prod() const {
		return d[1];
	}

	template <class F>
		requires IsSegmentTreeF<S, F>
	i32 max_right(i32 l, F f) const {
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

	template <class F>
		requires IsSegmentTreeF<S, F>
	i32 min_left(i32 r, F f) const {
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
	i32 n, size, log;
	std::vector<S> d;

	void update(i32 k) {
		d[k] = op(d[2 * k], d[2 * k + 1]);
	}
};

#endif // LIB_SEGMENT_TREE_HPP
