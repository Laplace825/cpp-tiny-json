/**
 * @author: Laplace825
 * @date: 2024-06-28T17:45:52
 * @lastmod: 2024-07-19T20:27:21
 * @description: the json object with all types
 * @filePath: /cpp-tiny-json/header-only/include/tjson/tjsonObj.hpp
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

class TJsonObj {
    friend std::ostream& operator<<(std::ostream& os, const TJsonObj& obj) {
        obj.print();
        return os;
    }

  public:
    using ListType   = std::vector< TJsonObj >;
    using DictType   = std::unordered_map< std::string, TJsonObj >;
    using value_type = std::variant< std::monostate, // null
      std::string,                                   // "String"
      ListType,                                      // [1,2, "ss", {}]
      DictType, // {"key":{"value":"Hello"}}
      double,   // 1.0
      int,      // 1 2 10
      bool      // true or false
      >;

  private:
    value_type m_value;

  protected:
    template < typename Callable >
    void call(const Callable& op) const {
        auto DoInvoke = [&](const auto&... args) { std::invoke(op, args...); };
        std::visit(
          [&](const auto& arg) {
              using T = std::decay_t< decltype(arg) >;
              if constexpr (std::is_same_v< T, std::monostate >) {
                  DoInvoke("null");
              }
              else if constexpr (std::is_same_v< T, std::string >) {
                  DoInvoke(std::format("\"{}\"", arg));
              }
              else if constexpr (std::is_same_v< T, double >) {
                  DoInvoke(arg);
              }
              else if constexpr (std::is_same_v< T, int >) {
                  DoInvoke(std::to_string(arg));
              }
              else if constexpr (std::is_same_v< T, bool >) {
                  DoInvoke(arg ? "true" : "false");
              }
              else if constexpr (std::is_same_v< T, ListType >) {
                  DoInvoke("[");
                  for (size_t i = 0; i < arg.size(); ++i) {
                      arg[i].call(op);
                      if (i + 1 < arg.size()) DoInvoke(", ");
                  }
                  DoInvoke("]");
              }
              else if constexpr (std::is_same_v< T, DictType >) {
                  DoInvoke("{");
                  for (auto it = arg.cbegin(); it != arg.cend(); ++it) {
                      DoInvoke(std::format("\"{}\": ", it->first));
                      it->second.call(op);
                      if (std::next(it) != arg.cend()) DoInvoke(", ");
                  }
                  DoInvoke("}");
              }
          },
          m_value);
    }

  public:
    TJsonObj() : m_value(std::monostate{}) {}

    const value_type& get() const { return m_value; }

    template < typename T >
    TJsonObj(T t) : m_value(std::move(t)) {}

    ~TJsonObj() = default;

    void println() const {
        this->print();
        std::cout << std::endl;
    }

    void print() const {
        call([](const auto&... arg) { ((std::cout << arg), ...); });
    }

    std::pair< DictType, std::string > toMap() const {
        std::ostringstream oss;
        DictType objMap;
        if (std::holds_alternative< DictType >(m_value)) {
            for (const auto& [key, value] : std::get< DictType >(m_value)) {
                objMap[key] = value;
            }
        }
        else {
            throw std::runtime_error(
              "\033[1;31mNot a DictType, maybe { or } is missing\033[0m");
        }
        call([&oss](
               const auto&... arg) { ((oss << std::format("{}", arg)), ...); });
        return {objMap, oss.str()};
    }

    // @note: since the TJsonObj<T>(T t) has implemented , this is deprecated
    // auto operator=(TJsonObj obj) -> TJsonObj& {
    //     std::swap(m_value, obj.m_value);
    //     return *this;
    // }

    template < typename T >
    auto operator=(T t) -> TJsonObj& {
        set(t);
        return *this;
    }

    template < typename T >
        requires(!std::is_same_v< T, TJsonObj >)
    void set(const T& src) {
        m_value = src;
    }

    auto operator[](size_t index) -> TJsonObj& {
        if (std::holds_alternative< ListType >(m_value)) {
            return std::get< ListType >(m_value)[index];
        }
        throw std::runtime_error(
          "\033[1;31mNot a ListType, can't use []\033[0m");
    }

    auto operator[](const std::string& key) -> TJsonObj& {
        if (std::holds_alternative< DictType >(m_value)) {
            return std::get< DictType >(m_value)[key];
        }
        throw std::runtime_error(
          "\033[1;31mNot a DictType, can't use []\033[0m");
    }

    bool operator==(const TJsonObj& obj) const {
        return m_value == obj.m_value;
    }

    std::string toString() const {
        std::stringstream oss;
        call([&oss](const auto&... arg) { ((oss << arg), ...); });
        return oss.str();
    }

    void clear() { m_value = std::monostate{}; }
};

} // namespace tjson
} // namespace lap

#endif // __TJSON_OBJ_HPP__
