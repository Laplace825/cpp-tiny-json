/**
 * @author: Laplace825
 * @date: 2024-06-28T17:43:53
 * @lastmod: 2024-07-20T17:31:29
 * @description: TJson class to handle json string with method integrating
 * @filePath: /cpp-tiny-json/header-only/include/tjson.hpp
 * @lastEditor: Laplace825
 * @ MIT lisence
 */

#ifndef __TJSON_HPP__
#define __TJSON_HPP__

#include <deque>
#include <ostream>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>

#include "tjson/tjsonObj.hpp"
#include "tjson/tjsonParser.hpp"
#include "tjson/tjsonToken.hpp"

namespace lap {

namespace tjson {

class TJson {
    friend std::ostream& operator<<(std::ostream& os, const TJson& obj) {
        obj.print();
        return os;
    }

  public:
    using Token = TJsonToken;

  protected:
    TJsonObj::NestingType m_json_dict;
    Parser m_parser;

  public:
    TJson()  = default;
    ~TJson() = default;

    explicit TJson(const std::string& json_str) : m_parser(json_str) {
        /***
         * @brief: will process and get the json to a unordered_map
         * @param: json_str {string_view}: your json string
         ***/
        for (const auto& [key, value] : m_parser.m_json_obj.toMap().first) {
            m_json_dict[key] = value;
        }
    }

    void setJsonStr(std::string json_str) {
        m_parser.set(std::move(json_str));
        for (const auto& [key, value] : m_parser.m_json_obj.toMap().first) {
            m_json_dict[key] = value;
        }
    }

    /**
     * @brief: BFS like search to find the key in json object
     * @param: key {string_view}: the key's value you want to find
     * @return: the reference of the value
     */
    TJsonObj& find(const std::string_view key) {
        if (m_json_dict.empty()) {
            throw std::runtime_error("json object is empty");
        }
        const TJsonObj* result{};
        using std::holds_alternative;

        auto bfs_search = [&]() {
            // BFS
            for (auto iter = m_json_dict.begin(); iter != m_json_dict.end();
              ++iter)
            {
                if (iter->first == key) {
                    return &iter->second;
                }
            }

            std::deque< const TJsonObj* > bfs_queue;
            bfs_queue.push_back(&m_json_dict.begin()->second);

            auto nested = [&](const TJsonObj::NestingType& nest_obj) {
                auto iter = nest_obj.find(key.data());
                if (iter != nest_obj.end()) {
                    result = &(iter->second);
                    return true; // find
                }
                return false;
            };

            auto listed = [&](const TJsonObj::ListType& list_obj) {
                for (auto element = list_obj.begin(); element != list_obj.end();
                  ++element)
                {
                    if (holds_alternative< TJsonObj::NestingType >( // only nest
                                                                    // to find
                          element->get()))
                    {
                        auto& nest_obj =
                          std::get< TJsonObj::NestingType >(element->get());
                        if (nested(nest_obj)) {
                            return true;
                        }
                    }
                }
                return false;
            };

            while (!bfs_queue.empty()) {
                auto bfs_front = bfs_queue.front();
                bfs_queue.pop_front();
                if (holds_alternative< TJsonObj::NestingType >(
                      bfs_front->get()))
                {
                    auto& nesting_obj =
                      std::get< TJsonObj::NestingType >(bfs_front->get());
                    if (nested(nesting_obj)) {
                        break;
                    }
                }
                else if (holds_alternative< TJsonObj::ListType >(
                           bfs_front->get()))
                {
                    auto& list_obj =
                      std::get< TJsonObj::ListType >(bfs_front->get());
                    if (listed(list_obj)) {
                        break;
                    }
                }
            }

            if (!result) {
                throw std::runtime_error("key not found");
            }
            return const_cast< TJsonObj* >(result);
        }();

        return *const_cast< TJsonObj* >(bfs_search);
    }

    /**
     * @brief: same as find method
     * @param: key {string_view}: the key's value you want to find
     * @return: the reference of the value
     */
    auto& operator[](const std::string_view key) { return find(key); }

    /**
     * @brief: reset to be empty,
     * you can use setJsonStr to set new json string
     */
    void clear() { m_json_dict.clear(); }

    void print() const {
        std::cout << "\n{";
        for (const auto& [key, value] : m_json_dict) {
            std::cout << "\n\t\"" << key << "\": ";
            value.println();
        }
        std::cout << "}";
    }

    void println() const {
        this->print();
        std::cout << std::endl;
    }

    std::string toString() const { return m_parser.m_json_obj.toString(); }

    auto cbegin() const { return m_json_dict.cbegin(); }

    auto cend() const { return m_json_dict.cend(); }
};

} // namespace tjson

} // namespace lap

#endif // __TJSON_HPP__
