/**
 * @author: Laplace825
 * @date: 2024-06-28T17:57:33
 * @lastmod: 2024-06-30T00:07:20
 * @description: a json parser
 * @filePath: /tiny-json/header-only/include/tjson/tjsonParser.hpp
 * @lastEditor: Laplace825
 * @ MIT license
 */
#ifndef __TJSON_PARSER_HPP__
#define __TJSON_PARSER_HPP__

#include <charconv>
#include <format>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <variant>

#include "tjson/tjsonObj.hpp"
#include "tjson/tjsonToken.hpp"

namespace lap {

namespace tjson {

// a Base Class to Provide all the static method,
// just a interface for Parser to use
class _ParserScan
{
  public:
    _ParserScan() = default;

    template < typename... Args >
    _ParserScan(Args&&... args) = delete;

    template < typename... Args >
    _ParserScan& operator=(Args&&... args) = delete;

  protected:
    static bool is_number_begin(char ch)
    {
        // number can't begin with '+'
        if (ch == '+')
            throw std::invalid_argument(
                "\033[1;31mnumber can't begin with +\033[0m");
        return ch >= '0' && ch <= '9' || ch == '-';
    }

    static bool is_string_begin(char ch) { return ch == '\"'; }

    static bool is_list_begin(char ch) { return ch == '['; }

    static bool is_json_nesting_begin(char ch) { return ch == '{'; }

    static bool is_boolean_true(char ch) { return ch == 't'; }

    static bool is_boolean_false(char ch) { return ch == 'f'; }

    static bool is_literal_null(char ch) { return ch == 'n'; }

    static bool is_whitespace(char ch)
    {
        return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
    }

    static char escape_char(char ch)
    {
        switch (ch)
        {
            case 'n': return '\n';
            case 't': return '\t';
            case 'r': return '\r';
            case 'b': return '\b';
            case 'f': return '\f';
            case '0': return '\0';
            case 'a': return '\a';
            case 'v': return '\v';
            default: return ch;
        }
    }

    static TJsonToken::Type scan_char(char json_begin = ' ')
    {
        /***
         * @description: scan the json string begin
         * and return the token type
         ***/
        if (json_begin == ' ')
        {
            return TJsonToken::END;
        }
        else if (is_number_begin(json_begin))
        {
            // a number
            return TJsonToken::VALUE_NUMBER;
        }
        else if (is_string_begin(json_begin))
        {
            // a string
            return TJsonToken::VALUE_STRING;
        }
        else if (is_list_begin(json_begin))
        {
            // a list
            return TJsonToken::LIST_BEGIN;
        }
        else if (is_json_nesting_begin(json_begin))
        {
            // a json object
            return TJsonToken::BEGIN_OBJECT;
        }
        else if (is_boolean_true(json_begin))
        {
            return TJsonToken::LITERAL_TRUE;
        }
        else if (is_boolean_false(json_begin))
        {
            return TJsonToken::LITERAL_FALSE;
        }
        else if (is_literal_null(json_begin))
        {
            return TJsonToken::LITERAL_NULL;
        }
        else
        {
            switch (json_begin)
            {
                case '}': return TJsonToken::END_OBJECT;
                case ',': return TJsonToken::VALUE_SEPRATOR;
                case ':': return TJsonToken::NAME_SEPRATOR;
                case ']': return TJsonToken::LIST_END;
                default: break;
            }
            return TJsonToken::END;
        }
    }

    static std::string escape_string(const std::string& str)
    {
        /***
         * @description: make the string with escape char
         ***/
        std::string res;
        for (size_t i = 0; i < str.size(); ++i)
        {
            if (str[i] == '\\')
            {
                if (i + 1 < str.size())
                {
                    res.push_back(escape_char(str[i + 1]));
                    ++i;
                }
            }
            else
            {
                res.push_back(str[i]);
            }
        }
        return res;
    }

    static std::size_t jump_whitespace(const std::string_view str,
                                       std::size_t begin) noexcept
    {
        for (; begin < str.size(); ++begin)
        {
            if (!is_whitespace(str[begin]))
            {
                break;
            }
        }
        return begin;
    }

    template < typename T >
    static std::optional< T > try_parse(std::string_view str) noexcept
    {
        T value{};
        auto res = std::from_chars(str.begin(), str.end(), value);
        if (res.ec == std::errc() && res.ptr == str.end())
        {
            return value;
        }
        return std::nullopt;
    }

    static void UpdateState(std::string& json_str, TJsonToken::Type& state,
                            std::size_t& reads, std::size_t begin)
    {
        reads    = jump_whitespace(json_str, begin);
        state    = scan_char(json_str[reads]);
        json_str = json_str.substr(reads);
    };

    static TJsonObj deal_value_string(std::string& json_str,
                                      TJsonToken::Type& state,
                                      std::size_t& reads)
    {
        // find the end of the json object
        auto find_res = json_str.find("\"");
        if (find_res != std::string::npos)
        {
            for (std::size_t i = find_res + 1; i < json_str.size(); i++)
            {
                if (json_str[i] == '\"')
                {
                    find_res = i;
                    break;
                }
            }
            // the json object to be implemented
            std::string res = json_str.substr(1, find_res - 1).data();
            json_str        = json_str.substr(find_res + 1);
            return TJsonObj{std::move(res)};
        }
        else
        {
            throw std::invalid_argument(
                "\033[1;31mjson string end with no \"\033[0m");
        }
    }

    static TJsonObj deal_value_number(std::string& json_str,
                                      TJsonToken::Type& state,
                                      std::size_t& reads)
    { // regex the float number and the scientific notation
        std::regex number_re{"[-]?[0-9]+(\\.[0-9]*)?([eE][+-]?[0-9]+)?"};

        //  store the match result
        std::match_results< decltype(json_str.begin()) > sub_match;

        if (std::regex_search(json_str.begin(), json_str.end(), sub_match,
                              number_re))
        {
            // if we find the number, then we try to parse it
            auto str = sub_match.str();
            if (auto value = try_parse< int >(str); value.has_value())
            {
                json_str = json_str.substr(str.size());
                return TJsonObj{value.value()};
            }
            if (auto value = try_parse< double >(str); value.has_value())
            {
                json_str = json_str.substr(str.size());
                return TJsonObj{value.value()};
            }
        }
        return TJsonObj{};
    }

    template < typename Callable >
    static TJsonObj deal_list(std::string& json_str, TJsonToken::Type& state,
                              std::size_t& reads, Callable&& op)
    {
        TJsonObj::ListType res{};
        reads = 1;
        while (reads < json_str.size())
        {
            UpdateState(json_str, state, reads, 1);

            if (json_str[0] == ']' && json_str.size() > 1)
            {
                state    = TJsonToken::LIST_END;
                json_str = json_str.substr(1);
                return TJsonObj{std::move(res)};
            }

            auto obj = std::forward< Callable >(op)(json_str, state, reads);

            res.push_back(std::move(obj));
        }
        return TJsonObj{std::move(res)};
    }

    template < typename Callable >
    static TJsonObj deal_obj_begin(std::string& json_str,
                                   TJsonToken::Type& state, std::size_t& reads,
                                   Callable&& op)
    {
        TJsonObj::NestingType res{};
        reads = 1;
        while (reads < json_str.size())
        {
            UpdateState(json_str, state, reads, 1);

            if (json_str[0] == '}' && json_str.size() > 1)
            {
                state    = TJsonToken::END_OBJECT;
                json_str = json_str.substr(1);
                return TJsonObj{std::move(res)};
            }
            if (json_str[0] == ':')
            {
                state    = TJsonToken::VALUE_SEPRATOR;
                json_str = json_str.substr(1);
                return std::forward< Callable >(op)(json_str, state, reads);
            }

            // get the key
            auto keyobj = std::forward< Callable >(op)(json_str, state, reads);

            if (std::holds_alternative< std::string >(keyobj.get()))
            {
                // to get the value,should make sure the map has the
                // key, then to find the value.
                res[std::get< std::string >(keyobj.get())] =
                    TJsonObj{std::monostate{}};
                UpdateState(json_str, state, reads, 1);
                auto valobj =
                    std::forward< Callable >(op)(json_str, state, reads);
                res[std::get< std::string >(keyobj.get())] = std::move(valobj);
            }
        }
        return TJsonObj{std::move(res)};
    }
};

class Parser : public _ParserScan
{
    friend class TJson;

  private:
    TJsonObj _json_obj;
    std::string _origin_str;

    static TJsonObj scanImpl(std::string& json_str, TJsonToken::Type& state,
                             std::size_t& reads)
    { /***
       * @param  reads {std::size_t}: how mach you have read
       * @param  state {TJsonTokenType}: use as a state machine
       * @return sttd::pair {*}: the json object and the reads
       * @exception: std::invalid_argument if the string can't be parsed
       * @description: scan the json string and return the json object
       ***/
        if (json_str.empty())
        {
            return TJsonObj{std::monostate{}};
        }

        switch (state)
        {
            case TJsonToken::VALUE_SEPRATOR: {
                UpdateState(json_str, state, reads, 1);
                return scanImpl(json_str, state, reads);
            }
            case TJsonToken::NAME_SEPRATOR: {
            }
            case TJsonToken::BEGIN_OBJECT: {
                return deal_obj_begin(json_str, state, reads, scanImpl);
            }
            case TJsonToken::VALUE_NUMBER: {
                return deal_value_number(json_str, state, reads);
            }
            case TJsonToken::VALUE_STRING: {
                return deal_value_string(json_str, state, reads);
            }
            case TJsonToken::LITERAL_TRUE: {
                if (json_str.substr(0, 4) == "true")
                {
                    json_str = json_str.substr(4);
                    return TJsonObj{true};
                }
                else
                {
                    throw std::invalid_argument(std::format(
                        "\033[1;31mjson boolean {} ,maybe you mean true\033[0m",
                        json_str.substr(0, 4)));
                }
                break;
            }
            case TJsonToken::LITERAL_FALSE: {
                if (json_str.substr(0, 5) == "false")
                {
                    json_str = json_str.substr(5);
                    return TJsonObj{false};
                }
                else
                {
                    throw std::invalid_argument(
                        std::format("\033[1;31mjson boolean {} ,maybe you mean "
                                    "false\033[0m",
                                    json_str.substr(0, 5)));
                }
                break;
            }
            case TJsonToken::LITERAL_NULL: {
                if (json_str.substr(0, 4) == "null")
                {
                    json_str = json_str.substr(4);
                    return TJsonObj{std::monostate{}};
                }
                else
                {
                    throw std::invalid_argument(
                        std::format("\033[1;31mjson NULL {} ,maybe you mean "
                                    "null\033[0m",
                                    json_str.substr(0, 4)));
                }
                break;
            }

            case TJsonToken::LIST_BEGIN: {
                return deal_list(json_str, state, reads, scanImpl);
            }
            default: break;
        }
        return TJsonObj{};
    }

  public:
    Parser() = default;

    Parser(std::string_view json_str)
        : _origin_str{escape_string(std::string(json_str))}
    {
        this->operator()();
    }

    void set(std::string json_str)
    {
        _origin_str = escape_string(std::move(json_str));
        this->operator()();
    }

    TJsonObj operator()()
    {
        std::string json_str   = _origin_str;
        std::size_t reads      = jump_whitespace(json_str, 0);
        TJsonToken::Type state = scan_char(json_str[reads]);
        json_str               = json_str.substr(reads);
        _json_obj              = scanImpl(json_str, state, reads);
        return _json_obj;
    }

    TJsonObj operator()(std::string_view json_str, bool isEscape = false)
    {
        _json_obj.set(escape_string(std::string(json_str)));
        return this->operator()();
    }

    TJsonObj scan() { return this->operator()(); }
};

} // namespace tjson

} // namespace lap

#endif // __TJSON_PARSER_HPP__
