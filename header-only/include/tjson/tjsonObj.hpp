/**
 * @author: Laplace825
 * @date: 2024-06-28T17:45:52
 * @lastmod: 2024-06-30T13:54:33
 * @description: the json object with all types
 * @filePath: /tiny-json/header-only/include/tjson/tjsonObj.hpp
 * @lastEditor: Laplace825
 * @ MIT lisence
 */

#ifndef __TJSON_OBJ_HPP__
#define __TJSON_OBJ_HPP__

#include <algorithm>
#include <format>
#include <functional>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace lap {

namespace tjson {

class TJsonObj
{
    friend std::ostream& operator<<(std::ostream& os, const TJsonObj& obj)
    {
        obj.print();
        return os;
    }

  public:
    using ListType    = std::vector< TJsonObj >;
    using NestingType = std::unordered_map< std::string, TJsonObj >;
    using value_type  = std::variant< std::monostate, // null
                                      std::string,    // "String"
                                      ListType,       // [1,2, "ss", {}]
                                      NestingType, // {"key":{"value":"Hello"}}
                                      double,      // 1.0
                                      int,         // 1 2 10
                                      bool         // true or false
                                      >;

  private:
    value_type _value;

  protected:
    template < typename Callable >
    void call(const Callable& op) const
    {
        auto DoInvoke = [&](const auto&... args) { std::invoke(op, args...); };

        std::visit(
            [&](auto&& arg) {
                using T = std::decay_t< decltype(arg) >;
                if constexpr (std::is_same_v< T, std::monostate >)
                {
                    DoInvoke("null");
                }
                else if constexpr (std::is_same_v< T, std::string >)
                {
                    DoInvoke(std::format("\"{}\"", arg));
                }
                else if constexpr (std::is_same_v< T, double >)
                {
                    DoInvoke(arg);
                }
                else if constexpr (std::is_same_v< T, int >)
                {
                    DoInvoke(std::to_string(arg));
                }
                else if constexpr (std::is_same_v< T, bool >)
                {
                    DoInvoke(arg ? "true" : "false");
                }
                else if constexpr (std::is_same_v< T, ListType >)
                {
                    DoInvoke("[");
                    for (size_t i = 0; i < arg.size(); ++i)
                    {
                        arg[i].call(op);
                        if (i + 1 < arg.size()) DoInvoke(", ");
                    }
                    DoInvoke("]");
                }
                else if constexpr (std::is_same_v< T, NestingType >)
                {
                    DoInvoke("{");
                    for (auto it = arg.begin(); it != arg.end(); ++it)
                    {
                        DoInvoke("\"", it->first, "\": ");
                        it->second.call(op);
                        if (std::next(it) != arg.end()) DoInvoke(", ");
                    }
                    DoInvoke("}");
                }
            },
            _value);
    }

  public:
    TJsonObj() : _value(std::monostate{}) {}

    auto& get() const { return _value; }

    template < typename T >
    explicit TJsonObj(T t) : _value(std::move(t))
    {
    }

    void println() const
    {
        this->print();
        std::cout << std::endl;
    }

    void print() const
    {
        call([](const auto&... arg) { ((std::cout << arg), ...); });
    }

    std::pair< NestingType, std::string > to_map() const
    {
        std::ostringstream oss;
        NestingType objMap;
        if (std::holds_alternative< NestingType >(_value))
        {
            for (const auto& [key, value] : std::get< NestingType >(_value))
            {
                objMap[key] = value;
            }
        }
        else
        {
            throw std::runtime_error(
                "\033[1;31mNot a DictType, maybe { or } is missing\033[0m");
        }
        call([&oss](const auto&... arg) {
            ((oss << std::format("{}", arg)), ...);
        });
        return {objMap, oss.str()};
    }

    auto operator=(TJsonObj obj) -> TJsonObj&
    {
        std::swap(_value, obj._value);
        return *this;
    }

    template < typename T >
        requires(!std::is_same_v< T, TJsonObj >)
    void set(const T& src)
    {
        _value = src;
    }

    auto operator[](size_t index) -> TJsonObj&
    {
        if (std::holds_alternative< ListType >(_value))
        {
            return std::get< ListType >(_value)[index];
        }
        throw std::runtime_error(
            "\033[1;31mNot a ListType, can't use []\033[0m");
    }

    auto operator[](const std::string& key) -> TJsonObj&
    {
        if (std::holds_alternative< NestingType >(_value))
        {
            return std::get< NestingType >(_value)[key];
        }
        throw std::runtime_error(
            "\033[1;31mNot a NestingType, can't use []\033[0m");
    }

    bool operator==(const TJsonObj& obj) const { return _value == obj._value; }

    std::string to_string() const
    {
        std::stringstream oss;
        call([&oss](const auto&... arg) { ((oss << arg), ...); });
        return oss.str();
    }
};

} // namespace tjson
} // namespace lap

#endif // __TJSON_OBJ_HPP__
