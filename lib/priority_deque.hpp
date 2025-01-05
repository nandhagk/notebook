#ifndef LIB_PRIORITY_DEQUE_HPP
#define LIB_PRIORITY_DEQUE_HPP 1

#include <vector>

#include <lib/prelude.hpp>

template <typename T>
struct priority_deque {
    std::vector<T> d;

    priority_deque() {}
    explicit priority_deque(const std::vector<T> &v) { build(v); }

    template <typename F>
    priority_deque(i32 n, F f) { build(n, f); }

    i32 size() const { return static_cast<i32>(d.size()); }
    bool empty() const { return d.empty(); }
    T min() const { return d[0]; }
    T max() const { return size() == 1 ? d[0] : d[1]; }

    void build(const std::vector<T> &v) {
        build(static_cast<i32>(v.size()), [&](i32 i) { return v[i]; });
    }

    template <typename F>
    void build(i32 n, F f) {
        d.resize(n);
        for (i32 i = 0; i < n; ++i) d[i] = f(i);

        for (i32 i = n - 1; i >= 0; --i) down(i);
    }

    void push(T x) { 
        d.push_back(x);
        up();
    }

    T pop_min() {
        assert(!empty());

        std::swap(d[0], d.back());

        const T res = d.back();
        d.pop_back();

        down(0);
        return res;
    }

    T pop_max() {
        assert(!empty());

        if (size() <= 2) {
            const T res = d.back();
            d.pop_back();

            return res;
        }

        std::swap(d[1], d.back());

        const T res = d.back();
        d.pop_back();

        down(1);
        return res;
    }

private:
    inline i32 parent(i32 i) { return (i - 4 + (i & 3)) / 2; }

    void down(i32 i) {
        const i32 n = size();
        if (i % 2 == 0) {
            while (1) {
                if (i + 1 < n && d[i] > d[i + 1]) std::swap(d[i], d[i + 1]);

                i32 j = i, l = 2 * i + 2, r = 2 * i + 4;
                if (l < n && d[l] < d[j]) j = l;
                if (r < n && d[r] < d[j]) j = r;
                if (i == j) break;

                std::swap(d[i], d[j]);
                i = j;
            }
        } else {
            for(;;) {
                if (d[i - 1] > d[i]) std::swap(d[i - 1], d[i]);

                i32 j = i, l = 2 * i + 1, r = 2 * i + 3;
                if (r >= n) --r;
                if (l >= n) --l;
                if (l < n && d[l] > d[j]) j = l;
                if (r < n && d[r] > d[j]) j = r;
                if (i == j) break;

                std::swap(d[i], d[j]);
                i = j;

                if (i % 2 == 0) break;
            }
        }
    }

    void up() {
        i32 i = size() - 1;
        if (2 <= i && i % 2 == 0) {
            const i32 p = parent(i) ^ 1;
            if (d[p] < d[i]) {
                std::swap(d[i], d[p]);
                i = p;
            }
        }

        if (i % 2 == 1 && d[i - 1] > d[i]) {
            std::swap(d[i - 1], d[i]);
            --i;
        }

        if (i % 2 == 0) {
            while (i >= 2) {
                const i32 p = parent(i);
                if (d[p] <= d[i]) break;

                std::swap(d[p], d[i]);
                i = p;
            }

            return;
        }

        while (i >= 3) {
            const i32 p = parent(i);
            if (d[p] >= d[i]) break;

            std::swap(d[p], d[i]);
            i = p;
        }
    }
};

#endif // LIB_PRIORITY_DEQUE_HPP
