#ifndef LIB_RABIN_KARP_HPP
#define LIB_RABIN_KARP_HPP 1

#include <functional>
#include <tuple>

#include <lib/prelude.hpp>
#include <lib/random.hpp>

template <typename... Zs>
struct tuple : public std::tuple<Zs...> {
    using std::tuple<Zs...>::tuple;

    friend constexpr tuple operator+(const tuple &t1, const tuple &t2) {
        return h(std::plus{}, t1, t2);
    }

    friend constexpr tuple operator-(const tuple &t1, const tuple &t2) {
        return h(std::minus{}, t1, t2);
    }

    friend constexpr tuple operator*(const tuple &t1, const tuple &t2) {
        return h(std::multiplies{}, t1, t2);
    }

    friend constexpr tuple operator/(const tuple &t1, const tuple &t2) {
        return h(std::divides{}, t1, t2);
    }

    static tuple rand() {
        return {static_cast<Zs>(MT())...};
    }

private:
    template <typename Op, typename... Ts, usize... Is>
    static constexpr tuple<Ts...> h(const Op &op, const tuple<Ts...> &t1, const tuple<Ts...> &t2,
                                    const std::index_sequence<Is...> &) {
        return {op(std::get<Is>(t1), std::get<Is>(t2))...};
    }

    template <typename Op, typename... Ts>
    static constexpr tuple<Ts...> h(const Op &op, const tuple<Ts...> &t1, const tuple<Ts...> &t2) {
        return h(op, t1, t2, std::index_sequence_for<Zs...>{});
    }

    template <typename Op, typename... Ts, usize... Is>
    static constexpr tuple<Ts...> h(const Op &op, const tuple<Ts...> &t1, const std::index_sequence<Is...> &) {
        return {op(std::get<Is>(t1))...};
    }

    template <typename Op, typename... Ts>
    static constexpr tuple<Ts...> h(const Op &op, const tuple<Ts...> &t1) {
        return h(op, t1, std::index_sequence_for<Zs...>{});
    }
};

template <typename... Zs>
struct rabin_karp {
    using Z = tuple<Zs...>;

    using X = std::pair<i32, Z>;
    using ValueT = X;

    static X op(const X &x, const X &y) {
        const auto &[a, b] = x;
        const auto &[p, q] = y;

        return {a + p, b + q * pow(a)};
    };

    static constexpr X unit() {
        return X{};
    }

    static constexpr X from_element(i32 c) {
        return {1, {static_cast<Zs>(c)...}};
    }

    static Z base;

    static constexpr bool commutative = false;

private:
    static Z pow(i32 n) {
        static std::vector<Z> pows{{static_cast<Zs>(1)...}};

        pows.reserve(n);
        while (n >= static_cast<i32>(pows.size())) pows.push_back(pows.back() * base);

        return pows[n];
    }
};

#endif // LIB_RABIN_KARP_HPP
