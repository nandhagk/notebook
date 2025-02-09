#ifndef LIB_AUGMENTED_LINK_CUT_TREE_HPP
#define LIB_AUGMENTED_LINK_CUT_TREE_HPP 1

#include <algorithm>
#include <cassert>

#include <lib/augmented_link_cut_tree_base.hpp>
#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_monoid_t<Monoid> * = nullptr>
struct augmented_link_cut_tree_node {
    struct MA {
        using ValueT = bool;
    };

    using MX = Monoid;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    augmented_link_cut_tree_node *hl, *hr, *ll, *lr, *p;
    X val, hsum, lsum, asum;
    bool rev, fake;
    i32 hsz, lsz, asz;

    augmented_link_cut_tree_node()
        : hl{nullptr},
          hr{nullptr},
          ll{nullptr},
          lr{nullptr},
          p{nullptr},
          val{MX::unit()},
          hsum{MX::unit()},
          lsum{MX::unit()},
          asum{MX::unit()},
          rev{false},
          fake{true},
          hsz{0},
          lsz{0},
          asz{0} {}

    explicit augmented_link_cut_tree_node(const X &x)
        : augmented_link_cut_tree_node() {
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

    void toggle() {
        std::swap(hl, hr);
        if constexpr (has_rev_v<MX>) hsum = MX::rev(hsum);
        rev ^= true;
    }

    void push() {
        if (rev) {
            if (hl != nullptr) hl->toggle();
            if (hr != nullptr) hr->toggle();
            rev = false;
        }
    }
};

template <typename ActedMonoid>
using augmented_link_cut_tree = augmented_link_cut_tree_base<augmented_link_cut_tree_node<ActedMonoid>>;

#endif // LIB_AUGMENTED_LINK_CUT_TREE_HPP
