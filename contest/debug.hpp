#ifndef LIB_DEBUG_HPP
#define LIB_DEBUG_HPP 1

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include <vector>

namespace __DEBUG_UTIL__ {

template <typename T>
concept is_iterable = requires(T &&x) { begin(x); } && !std::is_same_v<std::remove_cvref_t<T>, std::string>;

inline void print(const char *x) {
    std::cerr << x;
}

inline void print(char x) {
    std::cerr << "\'" << x << "\'";
}

inline void print(bool x) {
    std::cerr << (x ? 'T' : 'F');
}

inline void print(const std::string &x) {
    std::cerr << "\"" << x << "\"";
}

inline void print(std::string_view x) {
    std::cerr << "\"" << x << "\"";
}

inline void print(const std::vector<bool> &v) {
    int f = 0;

    std::cerr << '{';
    for (auto &&i : v) std::cerr << (f++ ? "," : "") << (i ? 'T' : 'F');
    std::cerr << '}';
}

template <typename T>
void print(T &&x) {
    if constexpr (is_iterable<T>) {
        if (std::size(x) && is_iterable<decltype(*(begin(x)))>) {
            int f = 0;
            std::cerr << "\n~~~~~\n";
            int w = std::max(0, (int)std::log10(std::size(x) - 1)) + 2;
            for (auto &&i : x) std::cerr << std::setw(w) << std::left << f++, print(i), std::cerr << '\n';
            std::cerr << "~~~~~\n";
        } else {
            int f = 0;
            std::cerr << '{';
            for (auto &&i : x) std::cerr << (f++ ? "," : ""), print(i);
            std::cerr << '}';
        }
    } else if constexpr (requires { x.pop(); }) {
        auto temp = x;

        int f = 0;
        std::cerr << '{';
        if constexpr (requires { x.top(); })
            while (!temp.empty()) std::cerr << (f++ ? "," : ""), print(temp.top()), temp.pop();
        else
            while (!temp.empty()) std::cerr << (f++ ? "," : ""), print(temp.front()), temp.pop();
        std::cerr << '}';
    } else if constexpr (requires {
                             x.first;
                             x.second;
                         }) {
        std::cerr << '(', print(x.first), std::cerr << ',', print(x.second), std::cerr << ')';
    } else if constexpr (requires { get<0>(x); }) {
        int f = 0;
        std::cerr << '(', apply([&f](auto... args) { ((std::cerr << (f++ ? "," : ""), print(args)), ...); }, x);
        std::cerr << ')';
    } else {
        std::cerr << x;
    }
}

template <typename T, typename... V>
void printer(const char *names, T &&head, V &&...tail) {
    int i = 0;
    for (int bracket = 0; names[i] != '\0' && (names[i] != ',' || bracket > 0); ++i)
        if (names[i] == '(' || names[i] == '<' || names[i] == '{')
            ++bracket;
        else if (names[i] == ')' || names[i] == '>' || names[i] == '}')
            --bracket;

    std::cerr.write(names, i) << " = ";
    print(head);

    if constexpr (sizeof...(tail))
        std::cerr << " ||", printer(names + i + 1, tail...);
    else
        std::cerr << "]\n";
}

} // namespace __DEBUG_UTIL__

#define debug(...) std::cerr << "[", __DEBUG_UTIL__::printer(#__VA_ARGS__, __VA_ARGS__)

#endif // LIB_DEBUG_HPP
