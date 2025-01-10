#ifndef LIB_PERSISTENT_QUEUE_HPP
#define LIB_PERSISTENT_QUEUE_HPP 1

#include <lib/persistent_array.hpp>
#include <lib/prelude.hpp>

template <typename T, i32 MAXN>
struct persistent_queue {
private:
    using self_t = persistent_queue;

    static constexpr i32 e() {
        return T();
    }

    using array_t = persistent_array<T, e, MAXN + 1>;

    i32 s{}, t{};
    array_t p;

    persistent_queue(i32 a, i32 b, array_t q) : s{a}, t{b}, p(q) {}

public:
    persistent_queue() {}

    self_t push_back(T x) const {
        return self_t(s, t + 1, p.set(t, x));
    }

    self_t pop_front() const {
        return self_t(s + 1, t, p);
    }

    T front() const {
        return p.get(s);
    }
};

#endif // LIB_PERSISTENT_QUEUE_HPP
