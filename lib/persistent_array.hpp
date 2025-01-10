#ifndef LIB_PERSISTENT_ARRAY_HPP
#define LIB_PERSISTENT_ARRAY_HPP 1

#include <array>
#include <functional>
#include <vector>

#include <lib/prelude.hpp>

constexpr i32 bit_ceil_log(i32 n) {
    i32 x = 0;
    while ((1 << x) < (i32)(n)) x++;
    return x;
}

template <typename T, auto e, i32 MAXIDX>
struct persistent_array {
    static_assert(std::is_convertible_v<decltype(e), std::function<T()>>);

private:
    using self_t = persistent_array<T, e, MAXIDX>;
    static constexpr i32 K = 2;
    static constexpr i32 log = (bit_ceil_log(MAXIDX) + K - 1) / K;
    static constexpr i32 nullid = -1;

    template <i32 dep, typename = void>
    struct node {};

    template <i32 dep>
    struct node<dep, std::enable_if_t<dep == log>> {
        T val;
        node()
            : val(e()) {}
    };

    template <i32 dep>
    struct node<dep, std::enable_if_t<dep != log>> {
        std::array<i32, 1 << K> ch;
        node() {
            ch.fill(nullid);
        }
    };

    template <i32 dep>
    struct node_vector {
        static std::vector<node<dep>> V;
        static i32 make_node() {
            i32 id = static_cast<i32>(V.size());
            V.push_back(node<dep>());
            return id;
        }
        static i32 copy_node(i32 id) {
            V.push_back(V[id]);
            id = static_cast<i32>(V.size()) - 1;
            return id;
        }
    };

    template <i32 dep>
    struct node_ref {
        i32 id;
        node_ref(i32 _id)
            : id(_id) {}
        node<dep> &operator*() {
            return node_vector<dep>::V[id];
        }
    };

    template <i32 dep>
    static i32 set(i32 id, i32 k, T x) {
        if (id == nullid)
            id = node_vector<dep>::make_node();
        else
            id = node_vector<dep>::copy_node(id);
        node_ref<dep> v(id);
        if constexpr (dep == log) {
            (*v).val = x;
            return id;
        } else {
            i32 dir = (k >> (K * (log - dep - 1))) % (1 << K);
            (*v).ch[dir] = set<dep + 1>((*v).ch[dir], k, x);
            return id;
        }
    }

    template <i32 dep>
    static T get(i32 id, i32 k) {
        if (id == nullid) return e();
        node_ref<dep> v(id);
        if constexpr (dep == log) {
            return (*v).val;
        } else {
            i32 dir = (k >> (K * (log - dep - 1))) % (1 << K);
            return get<dep + 1>((*v).ch[dir], k);
        }
    }

    template <i32 dep, std::enable_if_t<dep == log> * = nullptr>
    i32 build(const std::vector<T> &V, i32 L, i32) {
        if (L >= static_cast<i32>(V.size())) return -1;
        i32 id = node_vector<dep>::make_node();
        node_vector<dep>::V[id].val = V[L];
        return id;
    }

    template <i32 dep, std::enable_if_t<dep != log> * = nullptr>
    i32 build(const std::vector<T> &V, i32 L, i32 R) {
        if (L >= static_cast<i32>(V.size())) return -1;
        i32 id = node_vector<dep>::make_node();
        node_ref<dep> v(id);
        i32 M = (L + R) / 2;
        (*v).lch = build<dep + 1>(V, L, M);
        (*v).rch = build<dep + 1>(V, M, R);
        return id;
    }

    template <i32 dep, std::enable_if_t<dep == log> * = nullptr>
    static i32 copy(i32 id1, i32 id2, i32 l, i32 r, i32 L, i32 R) {
        if (r <= L || R <= l) return id1;
        return id2;
    }

    template <i32 dep, std::enable_if_t<dep != log> * = nullptr>
    static i32 copy(i32 id1, i32 id2, i32 l, i32 r, i32 L, i32 R) {
        if (r <= L || R <= l) return id1;
        if (l <= L && R <= r) return id2;
        if (id1 == nullid)
            id1 = node_vector<dep>::make_node();
        else
            id1 = node_vector<dep>::copy_node(id1);
        i32 M = (l + r) / 2;
        node_ref<dep> v1(id1);
        i32 lch2 = id2 == nullid ? nullid : node_vector<dep>::get_node(id2).lch;
        i32 rch2 = id2 == nullid ? nullid : node_vector<dep>::get_node(id2).rch;
        (*v1).lch = copy<dep + 1>((*v1).lch, lch2, L, M);
        (*v1).rch = copy<dep + 1>((*v1).rch, rch2, M, R);
        return id1;
    }

    i32 rootid;
    persistent_array(i32 _rootid)
        : rootid(_rootid) {}

public:
    persistent_array()
        : rootid(nullid) {}

    persistent_array(const std::vector<T> &V)
        : rootid(build<0>(V, 0, 1 << log)) {}

    self_t set(i32 k, T x) const {
        i32 id = set<0>(rootid, k, x);
        return self_t(id);
    }

    T get(i32 k) const {
        return get<0>(rootid, k);
    }

    T operator[](i32 k) const {
        return get(k);
    }

    self_t copy(self_t A, i32 l, i32 r) const {
        return copy<0>(rootid, A.rootid, l, r, 0, 1 << log);
    }
};

template <typename T, auto e, i32 MAXIDX>
template <i32 dep>
std::vector<typename persistent_array<T, e, MAXIDX>::template node<dep>>
    persistent_array<T, e, MAXIDX>::node_vector<dep>::V;

#endif // LIB_PERSISTENT_ARRAY_HPP
