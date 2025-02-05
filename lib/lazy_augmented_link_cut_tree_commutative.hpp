#ifndef LIB_LAZY_AUGMENTED_LINK_CUT_TREE_COMMUTATIVE_HPP
#define LIB_LAZY_AUGMENTED_LINK_CUT_TREE_COMMUTATIVE_HPP 1

#include <algorithm>
#include <cassert>

#include <lib/augmented_link_cut_tree_base.hpp>
#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename ActedMonoid>
struct lazy_augmented_link_cut_tree_commutative_node {
    using AM = ActedMonoid;

    using MX = typename AM::MX;
    using MA = typename AM::MA;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    lazy_augmented_link_cut_tree_commutative_node *hl, *hr, *ll, *lr, *p;
    X val, hsum, lsum, asum;
    A hlz, llz;
    bool rev, fake;
    i32 hsz, lsz, asz;

    lazy_augmented_link_cut_tree_commutative_node()
        : hl{nullptr},
          hr{nullptr},
          ll{nullptr},
          lr{nullptr},
          p{nullptr},
          val{MX::unit()},
          hsum{MX::unit()},
          lsum{MX::unit()},
          asum{MX::unit()},
          hlz{MA::unit()},
          llz{MA::unit()},
          rev{false},
          fake{true},
          hsz{0},
          lsz{0},
          asz{0} {}

    explicit lazy_augmented_link_cut_tree_commutative_node(const X &x)
        : lazy_augmented_link_cut_tree_commutative_node() {
        val = hsum = asum = x;
        fake = false;
        hsz = asz = 1;
    }

    bool is_root_heavy() {
        return !p || (p->hl != this && p->hr != this);
    }

    bool is_root_light() {
        return !p || (p->ll != this && p->lr != this) || !p->fake;
    }

    void update() {
        if (!fake) {
            hsz = 1;
            hsum = val;

            if (hl != nullptr) {
                hsz += hl->hsz;
                hsum = MX::op(hl->hsum, hsum);
            }

            if (hr != nullptr) {
                hsz += hr->hsz;
                hsum = MX::op(hsum, hr->hsum);
            }
        }

        lsz = 0;
        lsum = MX::unit();

        if (hl != nullptr) {
            lsz += hl->lsz;
            lsum = MX::op(hl->lsum, lsum);
        }

        if (hr != nullptr) {
            lsz += hr->lsz;
            lsum = MX::op(lsum, hr->lsum);
        }

        if (ll != nullptr) {
            lsz += ll->asz;
            lsum = MX::op(ll->asum, lsum);
        }

        if (lr != nullptr) {
            lsz += lr->asz;
            lsum = MX::op(lsum, lr->asum);
        }

        asz = hsz + lsz;
        asum = MX::op(hsum, lsum);
    }

    void push_heavy(const A &a) {
        if (fake) return;

        val = AM::act(val, a, 1);

        hlz = MA::op(hlz, a);
        hsum = AM::act(hsum, a, hsz);

        asz = hsz + lsz;
        asum = MX::op(hsum, lsum);
    }

    void push_light(bool o, const A &a) {
        llz = MA::op(llz, a);
        lsum = AM::act(lsum, a, lsz);

        if (!fake && o) {
            push_heavy(a);
        } else {
            asz = hsz + lsz;
            asum = MX::op(hsum, lsum);
        }
    }

    void toggle() {
        std::swap(hl, hr);
        rev ^= true;
    }

    void push() {
        if (hlz != MA::unit()) {
            if (hl != nullptr) hl->push_heavy(hlz);
            if (hr != nullptr) hr->push_heavy(hlz);
            hlz = MA::unit();
        }

        if (llz != MA::unit()) {
            if (hl != nullptr) hl->push_light(false, llz);
            if (hr != nullptr) hr->push_light(false, llz);
            if (ll != nullptr) ll->push_light(true, llz);
            if (lr != nullptr) lr->push_light(true, llz);
            llz = MA::unit();
        }

        if (rev) {
            if (hl != nullptr) hl->toggle();
            if (hr != nullptr) hr->toggle();
            rev = false;
        }
    }
};

template <typename ActedMonoid>
using lazy_augmented_link_cut_tree_commutative =
    augmented_link_cut_tree_base<lazy_augmented_link_cut_tree_commutative_node<ActedMonoid>>;

#endif // LIB_LAZY_AUGMENTED_LINK_CUT_TREE_COMMUTATIVE_HPP
