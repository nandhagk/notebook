#ifndef LIB_MATRIX_HPP
#define LIB_MATRIX_HPP 1

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <optional>
#include <valarray>

#include <lib/random.hpp>

template <typename vec, typename base>
struct valarray_base : std::valarray<base> {
    using Base = std::valarray<base>;
    using Base::Base;

    valarray_base(const base &t)
        : Base(t, 1) {}

    auto begin() {
        return std::begin(to_valarray());
    }

    auto begin() const {
        return std::begin(to_valarray());
    }

    auto end() {
        return std::end(to_valarray());
    }

    auto end() const {
        return std::end(to_valarray());
    }

    bool operator==(const vec &t) const {
        return std::equal(begin(), end(), t.begin(), t.end());
    }

    bool operator!=(const vec &t) const {
        return !(*this == t);
    }

    vec operator-() const {
        return Base::operator-();
    }

    Base &to_valarray() {
        return static_cast<Base &>(*this);
    }

    const Base &to_valarray() const {
        return static_cast<const Base &>(*this);
    }
};

template <typename vec, typename base>
vec operator+(const valarray_base<vec, base> &a, const valarray_base<vec, base> &b) {
    return a.to_valarray() + b.to_valarray();
}
template <typename vec, typename base>
vec operator-(const valarray_base<vec, base> &a, const valarray_base<vec, base> &b) {
    return a.to_valarray() - b.to_valarray();
}

template <typename base>
struct vec : valarray_base<vec<base>, base> {
    using Base = valarray_base<vec, base>;
    using Base::Base;

    static vec ei(usize n, usize i) {
        vec res(n);
        res[i] = 1;
        return res;
    }

    void add_scaled(const vec &b, base scale, usize i = 0) {
        if (scale != base(0))
            for (; i < size(*this); i++) (*this)[i] += scale * b[i];
    }

    const vec &normalize() {
        return static_cast<vec &>(*this);
    }

    base normalize(usize i) {
        return (*this)[i];
    }

    static vec random(usize n) {
        vec res(n);
        std::generate(res.begin(), res.end(), MT);

        return res;
    }

    // Concatenate vectors
    vec operator|(const vec &t) const {
        vec res(size(*this) + size(t));
        res[std::slice(0, size(*this), 1)] = *this;
        res[std::slice(size(*this), size(t), 1)] = t;
        return res;
    }

    // Generally, vec shouldn't be modified
    // after its pivot index is set
    std::pair<usize, base> find_pivot() {
        if (pivot == usize(-1)) {
            for (pivot = 0; pivot < size(*this) && normalize(pivot) == base(0); ++pivot);
            if (pivot < size(*this)) pivot_inv = base(1) / (*this)[pivot];
        }

        return {pivot, pivot_inv};
    }

    void reduce_by(vec &t) {
        const auto [p, pinv] = t.find_pivot();
        if (p < size(*this)) add_scaled(t, -normalize(p) * pinv, p);
    }

    friend std::istream &operator>>(std::istream &is, vec &v) {
        for (auto &it : v) is >> it;
        return is;
    }

    friend std::ostream &operator<<(std::ostream &os, const vec &v) {
        std::copy(v.begin(), v.end(), std::ostream_iterator<base>(os, " "));
        return os;
    }

private:
    usize pivot = -1;
    base pivot_inv;
};

enum gauss_mode { normal, reverse };

template <typename base_t>
struct matrix : valarray_base<matrix<base_t>, vec<base_t>> {
    using base = base_t;
    using Base = valarray_base<matrix<base>, vec<base>>;
    using Base::Base;

    explicit matrix(usize n)
        : Base(vec<base>(n), n) {}

    matrix(usize n, usize m)
        : Base(vec<base>(m), n) {}

    usize n() const {
        return size(*this);
    }

    usize m() const {
        return n() ? size(row(0)) : 0;
    }

    auto dim() const {
        return std::array{n(), m()};
    }

    auto &row(usize i) {
        return (*this)[i];
    }

    const auto &row(usize i) const {
        return (*this)[i];
    }

    matrix &operator*=(base t) {
        for (auto &it : *this) it *= t;
        return *this;
    }

    matrix operator*(base t) const {
        return matrix(*this) *= t;
    }

    matrix &operator/=(base t) {
        return *this *= base(1) / t;
    }

    matrix operator/(base t) const {
        return matrix(*this) /= t;
    }

    // Make sure the result is matrix, not Base
    matrix &operator*=(const matrix &t) {
        return *this = *this * t;
    }

    static matrix block_diagonal(const std::vector<matrix> &blocks) {
        usize n = 0;
        for (auto &it : blocks) {
            assert(it.n() == it.m());
            n += it.n();
        }

        matrix res(n);
        n = 0;
        for (auto &it : blocks) {
            for (usize i = 0; i < it.n(); ++i) res[n + i][std::slice(n, it.n(), 1)] = it[i];
            n += it.n();
        }

        return res;
    }

    static matrix random(usize n, usize m) {
        matrix res(n, m);
        std::generate(res.begin(), res.end(), std::bind(vec<base>::random, m));

        return res;
    }

    static matrix random(usize n) {
        return random(n, n);
    }

    static matrix eye(usize n) {
        matrix res(n);
        for (usize i = 0; i < n; ++i) res[i][i] = 1;
        return res;
    }

    // Concatenate matrices
    matrix operator|(const matrix &b) const {
        assert(n() == b.n());

        matrix res(n(), m() + b.m());
        for (usize i = 0; i < n(); ++i) res[i] = row(i) | b[i];

        return res;
    }

    template <typename T, typename U>
    matrix submatrix(T slicex, U slicey) const {
        matrix res = (*this)[slicex];
        for (auto &row : res) row = vec<base>(row[slicey]);

        return res;
    }

    matrix T() const {
        matrix res(m(), n());

        for (usize i = 0; i < n(); ++i)
            for (usize j = 0; j < m(); ++j) res[j][i] = row(i)[j];

        return res;
    }

    matrix operator*(const matrix &b) const {
        assert(m() == b.n());

        matrix res(n(), b.m());
        for (usize i = 0; i < n(); ++i)
            for (usize j = 0; j < m(); ++j) res[i].add_scaled(b[j], row(i)[j]);

        return res.normalize();
    }

    vec<base> apply(const vec<base> &x) const {
        return (matrix(x) * *this)[0];
    }

    matrix pow(u64 k) const {
        assert(n() == m());

        auto r = eye(n());
        for (auto b = *this; k; k >>= 1, b *= b)
            if (k & 1) r *= b;

        return r;
    }

    matrix &normalize() {
        for (auto &it : *this) it.normalize();
        return *this;
    }

    template <gauss_mode mode = normal>
    void eliminate(usize i, usize k) {
        const auto kinv = base(1) / row(i).normalize()[k];

        for (usize j = (mode == normal) * i; j < n(); ++j)
            if (j != i) row(j).add_scaled(row(i), -row(j).normalize(k) * kinv);
    }

    template <gauss_mode mode = normal>
    void eliminate(usize i) {
        row(i).normalize();
        for (usize j = (mode == normal) * i; j < n(); ++j)
            if (j != i) row(j).reduce_by(row(i));
    }

    template <gauss_mode mode = normal>
    matrix &gauss() {
        for (usize i = 0; i < n(); ++i) eliminate<mode>(i);
        return normalize();
    }

    template <gauss_mode mode = normal>
    auto echelonize(usize lim) {
        return gauss<mode>().sort_classify(lim);
    }

    template <gauss_mode mode = normal>
    auto echelonize() {
        return echelonize<mode>(m());
    }

    usize rank() const {
        if (n() > m()) return T().rank();
        return size(matrix(*this).echelonize()[0]);
    }

    base det() const {
        assert(n() == m());

        matrix b = *this;
        b.echelonize();

        base res = 1;
        for (usize i = 0; i < n(); ++i) res *= b[i][i];

        return res;
    }

    std::pair<base, matrix> inv() const {
        assert(n() == m());

        matrix b = *this | eye(n());
        if (size(b.echelonize<reverse>(n())[0]) < n()) return {0, {}};

        base det = 1;
        for (usize i = 0; i < n(); ++i) {
            det *= b[i][i];
            b[i] *= base(1) / b[i][i];
        }

        return {det, b.submatrix(std::slice(0, n(), 1), std::slice(n(), n(), 1))};
    }

    // Can also just run gauss on T() | eye(m)
    // but it would be slower :(
    auto kernel() const {
        auto A = *this;
        auto [pivots, free] = A.template echelonize<reverse>();

        matrix sols(size(free), m());
        for (usize j = 0; j < size(pivots); ++j) {
            base scale = base(1) / A[j][pivots[j]];
            for (usize i = 0; i < size(free); ++i) sols[i][pivots[j]] = A[j][free[i]] * scale;
        }

        for (usize i = 0; i < size(free); ++i) sols[i][free[i]] = -1;
        return sols;
    }

    // [solution, basis], transposed
    std::optional<std::array<matrix, 2>> solve(matrix t) const {
        matrix sols = (*this | t).kernel();

        if (sols.n() < t.m() ||
            sols.submatrix(std::slice(sols.n() - t.m(), t.m(), 1), std::slice(m(), t.m(), 1)) != -eye(t.m())) {
            return std::nullopt;
        } else {
            return std::array{sols.submatrix(std::slice(sols.n() - t.m(), t.m(), 1), std::slice(0, m(), 1)),
                              sols.submatrix(std::slice(0, sols.n() - t.m(), 1), std::slice(0, m(), 1))};
        }
    }

    // To be called after a gaussian elimination run
    // Sorts rows by pivots and classifies
    // variables i32o pivots and free
    auto sort_classify(usize lim) {
        usize rk = 0;
        std::vector<usize> free, pivots;

        for (usize j = 0; j < lim; ++j) {
            for (usize i = rk + 1; i < n() && row(rk)[j] == base(0); ++i) {
                if (row(i)[j] != base(0)) {
                    std::swap(row(i), row(rk));
                    row(rk) = -row(rk);
                }
            }

            if (rk < n() && row(rk)[j] != base(0)) {
                pivots.push_back(j);
                rk++;
            } else {
                free.push_back(j);
            }
        }

        return std::array{pivots, free};
    }

    base pfaffian() const {
        assert(n() == m());

        const i32 z = static_cast<i32>(n());
        auto A = *this;

        base res = 1;
        for (i32 i = 1; i < z; ++i) {
            for (i32 j = i + 1; j < z; ++j) {
                if (A[j].normalize(i - 1) == 0) continue;

                std::swap(A[i], A[j]);
                for (i32 k = i; k < z; ++k) std::swap(A[k][i], A[k][j]);

                res *= -1;
                break;
            }

            A[i].normalize();
            if (i % 2 == 1) res *= -A[i][i - 1];

            base Ai = 1 / A[i][i - 1];
            for (i32 j = i + 1; j < z; ++j) A[j].add_scaled(A[i], -A[j].normalize(i - 1) * Ai, i);
        }

        return res;
    }

    matrix adjugate() const {
        assert(n() == m());

        const i32 z = static_cast<i32>(n());
        matrix A(z + 1, z + 1);

        for (i32 i = 0; i < z; ++i)
            for (i32 j = 0; j < z; ++j) A[i][j] = (*this)[i][j];

        for (i32 i = 0; i < z; ++i) {
            A[i][z] = MT();
            A[z][i] = MT();
        }

        const auto [D, Ai] = A.inv();

        matrix B(z, z);
        if (D == 0) return B;

        for (i32 i = 0; i < z; ++i)
            for (i32 j = 0; j < z; ++j) B[i][j] = Ai[z][z] * Ai[i][j] - Ai[i][z] * Ai[z][j];

        return B * D;
    }

    friend std::istream &operator>>(std::istream &is, matrix &m) {
        for (auto &it : m) is >> it;
        return is;
    }

    friend std::ostream &operator<<(std::ostream &os, const matrix &m) {
        for (const auto &it : m) os << it << '\n';
        return os;
    }
};

template <typename base_t>
auto operator*(base_t t, const matrix<base_t> &A) {
    return A * t;
}

#endif // LIB_MATRIX_HPP
