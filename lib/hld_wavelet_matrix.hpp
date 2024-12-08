#ifndef LIB_HLD_WAVELET_MATRIX_HPP
#define LIB_HLD_WAVELET_MATRIX_HPP 1

#include <optional>

#include <lib/prelude.hpp>
#include <lib/hld.hpp>
#include <lib/wavelet_matrix.hpp>

template <typename T>
struct hld_wavelet_matrix {
	const hld& h;
	wavelet_matrix<T> wm;

	explicit hld_wavelet_matrix(const hld& g, const std::vector<T> &v): h(g) {
		build(v);
	}

	void build(const std::vector<T> &v) {
		std::vector<T> a(h.n);
		for (i32 u = 0; u < h.n; ++u) a[u] = v[h.tour[u]];

		wm.build(a);
	}

	i32 count_path(i32 u, i32 v, T a) const {
		i32 cnt{};

		for (const auto &[s, t] : h.decompose(u, v)) {
			const auto &[x, y] = std::minmax(h.tin[s], h.tin[t]);
			cnt += wm.count(x, y + 1, a);
		}

		return cnt;
	}

	i32 count_path(i32 u, i32 v, T a, T b) const {
		i32 cnt{};

		for (const auto &[s, t] : h.decompose(u, v)) {
			const auto &[x, y] = std::minmax(h.tin[s], h.tin[t]);
			cnt += wm.count(x, y + 1, a, b);
		}

		return cnt;
	}

	i32 count_subtree(i32 u, T a) const {
		return wm.count(h.tin[u], h.tin[u] + h.sz[u], a);
	}

	i32 count_subtree(i32 u, T a, T b) const {
		return wm.count(h.tin[u], h.tin[u] + h.sz[u], a, b);
	}

	T kth_path(i32 u, i32 v, i32 k) const {
		assert(0 <= k && k <= h.dist(u, v));

		std::vector<std::pair<i32, i32>> segments;
		for (const auto &[s, t] : h.decompose(u, v)) {
			const auto &[x, y] = std::minmax(h.tin[s], h.tin[t]);
			segments.emplace_back(x, y + 1);
		}

		i32 cnt{}, p{};
		for (i32 d = wm.log - 1; d >= 0; --d) {
			i32 c = 0;
			for (const auto &[l, r] : segments) {
				const i32 l0 = wm.bv[d].rank0(l);
				const i32 r0 = wm.bv[d].rank0(r);
				c += r0 - l0;
			}

			if (cnt + c > k) {
				for (auto &&[l, r] : segments) {
					const i32 l0 = wm.bv[d].rank0(l);
					const i32 r0 = wm.bv[d].rank0(r);

					l = l0;
					r = r0;
				}
			} else {
				cnt += c;
				p |= 1 << d;

				for (auto &&[l, r] : segments) {
					const i32 l0 = wm.bv[d].rank0(l);
					const i32 r0 = wm.bv[d].rank0(r);

					l += wm.md[d] - l0;
					r += wm.md[d] - r0;
				}
			}
		}

		return wm.rv[p];
	}

	T kth_subtree(i32 u, i32 k) const {
		return wm.kth(h.tin[u], h.tin[u] + h.sz[u], k);
	}

	std::optional<T> next_path(i32 u, i32 v, T a) const {
		const auto it = std::upper_bound(wm.rv.begin(), wm.rv.end(), a);
		if (it == wm.rv.end()) return a;

		const i32 k = count_path(u, v, *it);
		if (k == h.dist(u, v) + 1) return std::nullopt;

		return kth_path(u, v, k);
	}

	std::optional<T> prev_path(i32 u, i32 v, T a) const {
		const auto it = std::lower_bound(wm.rv.begin(), wm.rv.end(), a);
		if (it == wm.rv.begin()) return a;

		const i32 k = count_path(u, v, *it);
		if (k == 0) return std::nullopt;

		return kth_path(u, v, k - 1);
	}

	std::optional<T> next_subtree(i32 u, T a) const {
		const auto it = std::upper_bound(wm.rv.begin(), wm.rv.end(), a);
		if (it == wm.rv.end()) return a;

		const i32 k = count_subtree(u, *it);
		if (k == h.sz[u]) return std::nullopt;

		return kth_subtree(u, k);
	}

	std::optional<T> prev_subtree(i32 u, T a) const {
		const auto it = std::lower_bound(wm.rv.begin(), wm.rv.end(), a);
		if (it == wm.rv.begin()) return a;

		const i32 k = count_path(u, *it);
		if (k == 0) return std::nullopt;

		return kth_path(u, k - 1);
	}
};

#endif // LIB_HLD_WAVELET_MATRIX_HPP
