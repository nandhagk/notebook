#ifndef LIB_RBST_COMMUTATIVE_HPP
#define LIB_RBST_COMMUTATIVE_HPP 1

#include <algorithm>

#include <lib/prelude.hpp>
#include <lib/rbst_base.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_monoid_t<Monoid> * = nullptr>
struct rbst_commutative_node {
    struct MA {
        using ValueT = bool;
    };

    using MX = Monoid;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    rbst_commutative_node *l, *r;
    X val, sum;
    bool rev;
    i32 sz;

    explicit rbst_commutative_node(const X &x)
        : l{nullptr}, r{nullptr}, val{x}, sum{x}, rev{false}, sz{1} {}

    rbst_commutative_node()
        : rbst_commutative_node(MX::unit()) {}

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

    void toggle() {
        std::swap(l, r);
        rev ^= true;
    }

    void push() {
        if (rev) {
            if (l != nullptr) l->toggle();
            if (r != nullptr) r->toggle();
            rev = false;
        }
    }
};

template <typename ActedMonoid>
using rbst_commutative = rbst_base<rbst_commutative_node<ActedMonoid>>;

#endif // LIB_RBST_COMMUTATIVE_HPP
