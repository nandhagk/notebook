#ifndef LIB_TREAP_COMMUTATIVE_HPP
#define LIB_TREAP_COMMUTATIVE_HPP 1

#include <cassert>

#include <lib/prelude.hpp>
#include <lib/random.hpp>
#include <lib/treap_base.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_monoid_t<Monoid> * = nullptr>
struct treap_commutative_node {
    struct MA {
        using ValueT = bool;
    };

    using MX = Monoid;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    treap_commutative_node *l, *r;
    X val, sum;
    bool rev;
    i32 sz;
    u64 pr;

    explicit treap_commutative_node(const X &x)
        : l{nullptr}, r{nullptr}, val{x}, sum{x}, rev{false}, sz{1}, pr{MT()} {}

    treap_commutative_node()
        : treap_commutative_node(MX::unit()) {}

    treap_commutative_node *update() {
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

        return this;
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

template <typename Monoid>
using treap_commutative = treap_base<treap_commutative_node<Monoid>>;

#endif // LIB_TREAP_COMMUTATIVE_HPP
