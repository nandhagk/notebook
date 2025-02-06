#ifndef LIB_RBST_HPP
#define LIB_RBST_HPP 1

#include <algorithm>

#include <lib/prelude.hpp>
#include <lib/rbst_base.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_monoid_t<Monoid> * = nullptr>
struct rbst_node {
    struct MA {
        using ValueT = bool;
    };

    using MX = Monoid;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    rbst_node *l, *r;
    X val, sum, mus;
    bool rev;
    i32 sz;

    explicit rbst_node(const X &x)
        : l{nullptr}, r{nullptr}, val{x}, sum{x}, mus{x}, rev{false}, sz{1} {}

    rbst_node()
        : rbst_node(MX::unit()) {}

    rbst_node *update() {
        sz = 1;
        mus = sum = val;

        if (l != nullptr) {
            sz += l->sz;
            sum = MX::op(l->sum, sum);
            mus = MX::op(mus, l->mus);
        }

        if (r != nullptr) {
            sz += r->sz;
            sum = MX::op(sum, r->sum);
            mus = MX::op(r->mus, mus);
        }

        return this;
    }

    void toggle() {
        std::swap(l, r);
        std::swap(sum, mus);
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
using rbst = rbst_base<rbst_node<ActedMonoid>>;

#endif // LIB_RBST_HPP
