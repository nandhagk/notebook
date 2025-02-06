#ifndef LIB_LAZY_RBST_COMMUTATIVE_HPP
#define LIB_LAZY_RBST_COMMUTATIVE_HPP 1

#include <algorithm>

#include <lib/prelude.hpp>
#include <lib/rbst_base.hpp>
#include <lib/type_traits.hpp>

template <typename ActedMonoid>
struct lazy_rbst_commutative_node {
    using AM = ActedMonoid;

    using MX = typename AM::MX;
    using MA = typename AM::MA;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    lazy_rbst_commutative_node *l, *r;
    X val, sum;
    A lz;
    bool rev;
    i32 sz;

    explicit lazy_rbst_commutative_node(const X &x)
        : l{nullptr}, r{nullptr}, val{x}, sum{x}, lz{MA::unit()}, rev{false}, sz{1} {}

    lazy_rbst_commutative_node()
        : lazy_rbst_commutative_node(MX::unit()) {}

    void update() {
        sz = 1;
        sum = val;

        if (l != nullptr) {
            sz += l->sz;
            sum = MX::op(l->sum, sum);
        }

        if (r != nullptr) {
            sz += r->sz;
            sum = MX::op(sum, r->sum);
        }
    }

    void all_apply(const A &a) {
        val = AM::act(val, a, 1);
        lz = MA::op(lz, a);

        sum = AM::act(sum, a, sz);

        if constexpr (has_fail_v<MX>) {
            if (MX::failed(sum)) {
                push();
                update();
            }
        }
    }

    void toggle() {
        std::swap(l, r);
        rev ^= true;
    }

    void push() {
        if (lz != MA::unit()) {
            if (l != nullptr) l->all_apply(lz);
            if (r != nullptr) r->all_apply(lz);
            lz = MA::unit();
        }

        if (rev) {
            if (l != nullptr) l->toggle();
            if (r != nullptr) r->toggle();
            rev = false;
        }
    }
};

template <typename ActedMonoid>
using lazy_rbst_commutative = rbst_base<lazy_rbst_commutative_node<ActedMonoid>>;

#endif // LIB_LAZY_RBST_COMMUTATIVE_HPP
