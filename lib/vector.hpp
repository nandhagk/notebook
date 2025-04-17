#ifndef LIB_VECTOR_HPP
#define LIB_VECTOR_HPP 1

#include <algorithm>
#include <iostream>
#include <iterator>
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

#endif // LIB_VECTOR_HPP
