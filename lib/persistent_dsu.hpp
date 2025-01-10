#ifndef LIB_PERSISTENT_DSU_HPP
#define LIB_PERSISTENT_DSU_HPP 1

#include <lib/persistent_array.hpp>
#include <lib/prelude.hpp>

template <i32 MAXN>
struct persistent_dsu {
private:
    using self_t = persistent_dsu;

    static constexpr i32 e() { return -1; }

    using array_t = persistent_array<i32, e, MAXN + 1>;

    std::pair<i32, i32> get_info(i32 u) const {
        i32 s = sz[u];
        if (s < 0) return {u, -s};
        return get_info(s);
    }

    array_t sz;
    persistent_dsu(array_t _sz) : sz(_sz) {}

public:
    persistent_dsu() {}

    i32 find(i32 u) const { return get_info(u).first; }

    i32 size(i32 u) const { return get_info(u).second; }

    bool same(i32 u, i32 v) const { return find(u) == find(v); }

    self_t merge(i32 u, i32 v) const {
        i32 usz, vsz;
        std::tie(u, usz) = get_info(u);
        std::tie(v, vsz) = get_info(v);
        if (u == v) return *this;
        if (usz < vsz) std::swap(u, v);
        array_t new_sz = sz;
        new_sz = new_sz.set(v, u);
        new_sz = new_sz.set(u, -(usz + vsz));
        return self_t(new_sz);
    }
};

#endif // LIB_PERSISTENT_DSU_HPP
