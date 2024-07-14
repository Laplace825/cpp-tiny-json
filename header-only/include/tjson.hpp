/**
 * @author: Laplace825
 * @date: 2024-06-28T17:43:53
 * @lastmod: 2024-06-30T21:00:54
 * @description: TJson class to handle json string with method integrating
 * @filePath: /tiny-json/header-only/include/tjson.hpp
 * @lastEditor: Laplace825
 * @ MIT lisence
 */

#ifndef __TJSON_HPP__
#define __TJSON_HPP__

#include "tjson/tjsonObj.hpp"
#include "tjson/tjsonParser.hpp"
#include "tjson/tjsonToken.hpp"

#include <ostream>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>

namespace lap {

namespace tjson {

class TJson
{
    friend std::ostream& operator<<(std::ostream& os, const TJson& obj)
    {
        obj.print();
        return os;
    }

  public:
    using Token = TJsonToken;

  protected:
    TJsonObj::NestingType m_json_dict;
    Parser m_parser;

  public:
    TJson() = default;

    explicit TJson(const std::string& json_str) : m_parser(json_str)
    {
        /***
         * @param json_str {string_view}: your json string
         * @description: will process and get the json to a unordered_map
         ***/
        for (const auto& [key, value] : m_parser.m_json_obj.to_map().first)
        {
            m_json_dict[key] = value;
        }
    }

    void setJsonStr(std::string json_str)
    {
        m_parser.set(std::move(json_str));
        for (const auto& [key, value] : m_parser.m_json_obj.to_map().first)
        {
            m_json_dict[key] = value;
        }
    }

    auto operator[](const std::string_view key) const { return find(key); }

    // recursive find key's value
    // dfs like
    TJsonObj find(const std::string_view key) const
    {
        if (m_json_dict.empty())
        {
            return TJsonObj{};
        }
        TJsonObj result;
        auto findImpl = [&](auto&& self_func,
                            const TJsonObj::NestingType& _json_str_map,
                            const TJsonObj::ListType& _json_str_list = {},
                            Token::Type isList =
                                Token::VALUE_JSON_ELEMENT) -> TJsonObj {
            switch (isList)
            {
                case Token::VALUE_JSON_ELEMENT: {
                    for (auto iter = _json_str_map.begin();
                         iter != _json_str_map.end();
                         ++iter)
                    {
                        if (iter->first == key)
                        {
                            result = iter->second;
                            break;
                        }
                        else if (std::holds_alternative<
                                     TJsonObj::NestingType >(
                                     iter->second.get()))
                        {
                            self_func(self_func,
                                std::get< TJsonObj::NestingType >(
                                    iter->second.get()),
                                _json_str_list,
                                Token::VALUE_JSON_ELEMENT);
                        }
                        else if (std::holds_alternative< TJsonObj::ListType >(
                                     iter->second.get()))
                        {
                            self_func(self_func,
                                _json_str_map,
                                std::get< TJsonObj::ListType >(
                                    iter->second.get()),
                                Token::LIST_BEGIN);
                        }
                    }
                    return result;
                }
                case Token::LIST_BEGIN: {
                    // Handling list elements
                    for (auto& element : _json_str_list)
                    {
                        if (std::holds_alternative< TJsonObj::NestingType >(
                                element.get()))
                        {
                            self_func(self_func,
                                std::get< TJsonObj::NestingType >(
                                    element.get()),
                                _json_str_list,
                                Token::VALUE_JSON_ELEMENT);
                        }
                        else if (std::holds_alternative< TJsonObj::ListType >(
                                     element.get()))
                        {
                            self_func(self_func,
                                _json_str_map,
                                std::get< TJsonObj::ListType >(element.get()),
                                Token::LIST_BEGIN);
                        }
                    }
                    return result;
                }
                default: break;
            }
            return result;
        };
        result = findImpl(findImpl, m_json_dict);
        return std::holds_alternative< std::monostate >(result.get())
                   ? TJsonObj{}
                   : result;
    }

    // reset to be empty
    // use setJsonStr to set new json string
    void clear() { m_json_dict.clear(); }

    void print() const
    {
        std::cout << "\n{";
        for (const auto& [key, value] : m_json_dict)
        {
            std::cout << "\n\t\"" << key << "\": ";
            value.println();
        }
        std::cout << "}";
    }

    void println() const
    {
        this->print();
        std::cout << std::endl;
    }

    std::string to_string() const { return m_parser.m_json_obj.to_string(); }
};

} // namespace tjson

} // namespace lap

#endif // __TJSON_HPP__
