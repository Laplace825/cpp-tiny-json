/**
 * @author: Laplace825
 * @date: 2024-06-28T17:57:33
 * @lastmod: 2024-07-19T01:27:32
 * @description: a json parser
 * @filePath: /cpp-tiny-json/header-only/include/tjson/tjsonParser.hpp
 * @lastEditor: Laplace825
 * @ MIT license
 */
#ifndef __TJSON_PARSER_HPP__
#define __TJSON_PARSER_HPP__

#include <format>
#include <stdexcept>
#include <string_view>
#include <variant>

#include "tjson/detail/_ParserScan.hpp"
#include "tjson/detail/_TJsonToken.hpp"
#include "tjson/tjsonObj.hpp"

namespace lap {

namespace tjson {

class Parser {
    friend class TJson;

  private:
    TJsonObj m_json_obj;
    std::string m_origin_str;

    static TJsonObj scanImpl(std::string& json_str,
      __detail::_TJsonToken::Type& state,
      std::size_t&
        reads) { /***
                  * @param  reads {std::size_t}: how mach you have read
                  * @param  state {__detail::_TJsonTokenType}: use as a state
                  *machine
                  * @return sttd::pair {*}: the json object and the reads
                  * @exception: std::invalid_argument if the string can't be
                  *parsed
                  * @description: scan the json string and return the json
                  *object
                  ***/
        using namespace __detail::_ParserScan;
        if (json_str.empty()) {
            return TJsonObj{std::monostate{}};
        }

        switch (state) {
            case __detail::_TJsonToken::VALUE_SEPRATOR: {
                update_state(json_str, state, reads, 1);
                return scanImpl(json_str, state, reads);
            }
            case __detail::_TJsonToken::NAME_SEPRATOR: {
            }
            case __detail::_TJsonToken::BEGIN_OBJECT: {
                return dealObjBegin(json_str, state, reads, scanImpl);
            }
            case __detail::_TJsonToken::VALUE_NUMBER: {
                return dealValueNumber(json_str, state, reads);
            }
            case __detail::_TJsonToken::VALUE_STRING: {
                return dealValueString(json_str, state, reads);
            }
            case __detail::_TJsonToken::LITERAL_TRUE: {
                if (json_str.substr(0, 4) == "true") {
                    json_str = json_str.substr(4);
                    return TJsonObj{true};
                }
                else {
                    throw std::invalid_argument(std::format(
                      "\033[1;31mjson boolean {} ,maybe you mean true\033[0m",
                      json_str.substr(0, 4)));
                }
                break;
            }
            case __detail::_TJsonToken::LITERAL_FALSE: {
                if (json_str.substr(0, 5) == "false") {
                    json_str = json_str.substr(5);
                    return TJsonObj{false};
                }
                else {
                    throw std::invalid_argument(
                      std::format("\033[1;31mjson boolean {} ,maybe you mean "
                                  "false\033[0m",
                        json_str.substr(0, 5)));
                }
                break;
            }
            case __detail::_TJsonToken::LITERAL_NULL: {
                if (json_str.substr(0, 4) == "null") {
                    json_str = json_str.substr(4);
                    return TJsonObj{std::monostate{}};
                }
                else {
                    throw std::invalid_argument(
                      std::format("\033[1;31mjson NULL {} ,maybe you mean "
                                  "null\033[0m",
                        json_str.substr(0, 4)));
                }
                break;
            }
            case __detail::_TJsonToken::LIST_BEGIN: {
                return deaList(json_str, state, reads, scanImpl);
            }
            default:
                break;
        }
        return TJsonObj{};
    }

  public:
    Parser()  = default;
    ~Parser() = default;

    Parser(std::string json_str)
        : m_origin_str{__detail::_ParserScan::escapeString(json_str)} {
        this->operator()();
    }

    void set(std::string json_str) {
        m_origin_str = __detail::_ParserScan::escapeString(json_str);
        this->operator()();
    }

    TJsonObj operator()() {
        std::string json_str = m_origin_str;
        std::size_t reads = __detail::_ParserScan::jumpWhiteSpace(json_str, 0);
        __detail::_TJsonToken::Type state =
          __detail::_ParserScan::scanChar(json_str[reads]);
        json_str   = json_str.substr(reads);
        m_json_obj = scanImpl(json_str, state, reads);
        return m_json_obj;
    }

    TJsonObj operator()(std::string_view json_str) {
        m_json_obj.set(
          __detail::_ParserScan::escapeString(std::string(json_str)));
        return this->operator()();
    }

    TJsonObj scan() { return this->operator()(); }

    void clear() {
        m_json_obj.clear();
        m_origin_str.clear();
    }
};

} // namespace tjson

} // namespace lap

#endif // __TJSON_PARSER_HPP__
