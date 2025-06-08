#ifndef LIB_CSR_ARRAY_HPP
#define LIB_CSR_ARRAY_HPP 1

#include <numeric>
#include <vector>

#include <lib/prelude.hpp>

template <typename T>
struct csr_array {
    csr_array() {}
    csr_array(i32 p, const std::vector<std::pair<i32, T>> &d) {
        build(p, d);
    }

    void build(i32 p, const std::vector<std::pair<i32, T>> &d) {
        n = p;
        m = static_cast<i32>(d.size());

        start.resize(n);
        for (const auto &[u, _] : d) ++start[u];

        sz = start;
        std::partial_sum(start.cbegin(), start.cend(), start.begin());

        arr.resize(m);
        for (const auto &[u, e] : d) arr[--start[u]] = e;
    }

    usize size() const {
        return n;
    }

    struct span {
        const T *d;
        usize sz;

        span(const T *e, usize size)
            : d{e}, sz{size} {}

        const T *begin() {
            return &d[0];
        }

        const T *end() {
            return &d[sz];
        }

        const T *begin() const {
            return &d[0];
        }

        const T *end() const {
            return &d[sz];
        }

        const T &operator[](usize i) const {
            return d[i];
        }

        usize size() const {
            return sz;
        }

        bool empty() const {
            return sz == 0;
        }
    };

    span operator[](i32 u) const {
        return span(arr.data() + start[u], sz[u]);
    }

    i32 n, m;

private:
    std::vector<i32> start, sz;
    std::vector<T> arr;
};

#endif // LIB_CSR_ARRAY_HPP 1
