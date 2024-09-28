#ifndef LIB_DEBUG_HPP
#define LIB_DEBUG_HPP 1

#include <type_traits>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>

namespace __DEBUG_UTIL__
{
    template <typename T>
    concept is_iterable = requires(T &&x) { begin(x); } &&
                          !std::is_same_v<std::remove_cvref_t<T>, std::string>;
    inline void print(const char *x) { std::cerr << x; }
    inline void print(char x) { std::cerr << "\'" << x << "\'"; }
    inline void print(bool x) { std::cerr << (x ? "T" : "F"); }
    inline void print(std::string x) { std::cerr << "\"" << x << "\""; }
    inline void print(std::vector<bool> &v)
    { /* Overloaded this because stl optimizes vector<bool> by using
         _Bit_reference instead of bool to conserve space. */
        int f = 0;
        std::cerr << '{';
        for (auto &&i : v)
            std::cerr << (f++ ? "," : "") << (i ? "T" : "F");
        std::cerr << "}";
    }
    template <typename T>
    void print(T &&x)
    {
        if constexpr (is_iterable<T>)
            if (std::size(x) && is_iterable<decltype(*(begin(x)))>)
            { /* Iterable inside Iterable */
                int f = 0;
                std::cerr << "\n~~~~~\n";
                int w = std::max(0, (int)std::log10(std::size(x) - 1)) + 2;
                for (auto &&i : x)
                {
                    std::cerr << std::setw(w) << std::left << f++, print(i), std::cerr << "\n";
                }
                std::cerr << "~~~~~\n";
            }
            else
            { /* Normal Iterable */
                int f = 0;
                std::cerr << "{";
                for (auto &&i : x)
                    std::cerr << (f++ ? "," : ""), print(i);
                std::cerr << "}";
            }
        else if constexpr (requires { x.pop(); }) /* Stacks, Priority Queues, Queues */
        {
            auto temp = x;
            int f = 0;
            std::cerr << "{";
            if constexpr (requires { x.top(); })
                while (!temp.empty())
                    std::cerr << (f++ ? "," : ""), print(temp.top()), temp.pop();
            else
                while (!temp.empty())
                    std::cerr << (f++ ? "," : ""), print(temp.front()), temp.pop();
            std::cerr << "}";
        }
        else if constexpr (requires { x.first; x.second; }) /* Pair */
        {
            std::cerr << '(', print(x.first), std::cerr << ',', print(x.second), std::cerr << ')';
        }
        else if constexpr (requires { get<0>(x); }) /* Tuple */
        {
            int f = 0;
            std::cerr << '(', apply([&f](auto... args)
                               { ((std::cerr << (f++ ? "," : ""), print(args)), ...); },
                               x);
            std::cerr << ')';
        }
        else
            std::cerr << x;
    }
    template <typename T, typename... V>
    void printer(const char *names, T &&head, V &&...tail)
    {
        int i = 0;
        for (int bracket = 0; names[i] != '\0' and (names[i] != ',' or bracket > 0); i++)
            if (names[i] == '(' or names[i] == '<' or names[i] == '{')
                bracket++;
            else if (names[i] == ')' or names[i] == '>' or names[i] == '}')
                bracket--;
        std::cerr.write(names, i) << " = ";
        print(head);
        if constexpr (sizeof...(tail))
            std::cerr << " ||", printer(names + i + 1, tail...);
        else
            std::cerr << "]\n";
    }

}

#define debug(...) std::cerr << "[", __DEBUG_UTIL__::printer(#__VA_ARGS__, __VA_ARGS__)

#endif // LIB_DEBUG_HPP
