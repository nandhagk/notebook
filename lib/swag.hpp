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
    std::vector<X> sum_left, sum_right;

    swag() { build(); }

    void build() {
        q.clear();
        sum_left.clear();
        sum_right.clear();

        sum_left.push_back(MX::unit());
        sum_right.push_back(MX::unit());
    }

    i32 size() const { return static_cast<i32>(q.size()); }

    X prod_all() const { return MX::op(sum_left.back(), sum_right.back()); }

    void push_back(X x) {
        q.push_back(x);
        sum_right.push_back(MX::op(sum_right.back(), x));
    }

    void pop_back() {
        assert(size());

        if (sum_right.size() > 1) {
            sum_right.pop_back();
            q.pop_back();

            return;
        }

        const i32 new_r_size = (size() + 1) / 2;
        const i32 new_l_size = size() - new_r_size;
        q.pop_back();

        for (i32 i = new_l_size; i < size(); ++i) sum_right.push_back(MX::op(sum_right.back(), q[i]));

        sum_left.resize(new_l_size + 1);
        for (i32 i = 0; i < new_l_size; ++i) sum_left[i + 1] = MX::op(q[new_l_size - 1 - i], sum_left[i]);
    }

    void push_front(X x) {
        q.push_front(x);
        sum_left.push_back(MX::op(x, sum_left.back()));
    }

    void pop_front() {
        assert(size());

        if (sum_left.size() > 1) {
            sum_left.pop_back();
            q.pop_front();

            return;
        }

        const i32 new_l_size = (size() + 1) / 2;
        const i32 new_r_size = size() - new_l_size;

        for (i32 i = 0; i < new_l_size - 1; ++i) sum_left.push_back(MX::op(q[new_l_size - 1 - i], sum_left[i]));

        sum_right.resize(new_r_size + 1);
        for (i32 i = 0; i < new_r_size; ++i) sum_right[i + 1] = MX::op(sum_right[i], q[new_l_size + i]);

        q.pop_front();
    }

    X back() const { return q.back(); }
    X front() const { return q.front(); }
};

#endif // LIB_SWAG_HPP
