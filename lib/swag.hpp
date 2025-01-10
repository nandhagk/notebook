#ifndef LIB_SWAG_HPP
#define LIB_SWAG_HPP 1

#include <deque>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_monoid_t<Monoid> * = nullptr>
struct swag {
    using MX = Monoid;
    using X = typename MX::ValueT;

    std::deque<X> q;
    std::vector<X> suml, sumr;

    swag() {
        build();
    }

    void build() {
        q.clear();
        suml.clear();
        sumr.clear();

        suml.push_back(MX::unit());
        sumr.push_back(MX::unit());
    }

    i32 size() const {
        return static_cast<i32>(q.size());
    }

    X prod_all() const {
        return MX::op(suml.back(), sumr.back());
    }

    void push_back(X x) {
        q.push_back(x);
        sumr.push_back(MX::op(sumr.back(), x));
    }

    void pop_back() {
        assert(size());

        if (sumr.size() > 1) {
            sumr.pop_back();
            q.pop_back();

            return;
        }

        const i32 rsz = (size() + 1) / 2;
        const i32 lsz = size() - rsz;
        q.pop_back();

        for (i32 i = lsz; i < size(); ++i) sumr.push_back(MX::op(sumr.back(), q[i]));

        suml.resize(lsz + 1);
        for (i32 i = 0; i < lsz; ++i) suml[i + 1] = MX::op(q[lsz - 1 - i], suml[i]);
    }

    void push_front(X x) {
        q.push_front(x);
        suml.push_back(MX::op(x, suml.back()));
    }

    void pop_front() {
        assert(size());

        if (suml.size() > 1) {
            suml.pop_back();
            q.pop_front();

            return;
        }

        const i32 lsz = (size() + 1) / 2;
        const i32 rsz = size() - lsz;

        for (i32 i = 0; i < lsz - 1; ++i) suml.push_back(MX::op(q[lsz - 1 - i], suml[i]));

        sumr.resize(rsz + 1);
        for (i32 i = 0; i < rsz; ++i) sumr[i + 1] = MX::op(sumr[i], q[lsz + i]);

        q.pop_front();
    }

    X back() const {
        return q.back();
    }
    X front() const {
        return q.front();
    }
};

#endif // LIB_SWAG_HPP
