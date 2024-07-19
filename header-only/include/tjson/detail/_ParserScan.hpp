/**
 * @author: Laplace825
 * @date: 2024-07-18T19:05:12
 * @lastmod: 2024-07-19T20:27:57
 * @description: the detail of tjson implementation
 * @filePath: /cpp-tiny-json/header-only/include/tjson/detail/_ParserScan.hpp
 * @lastEditor: Laplace825
 * @ MIT license
 */

#ifndef __TJSON_DETAIL_HPP__
#define __TJSON_DETAIL_HPP__

#include <charconv>
#include <regex>

#include "_TJsonToken.hpp"
#include "tjson/tjsonObj.hpp"

namespace lap {

namespace tjson {

namespace __detail {

namespace _ParserScan {

bool isNumberBegin(char ch) {
    // number can't begin with '+'
    if (ch == '+')
        throw std::invalid_argument(
          "\033[1;31mnumber can't begin with +\033[0m");
    return (ch >= '0' && ch <= '9') || ch == '-';
}

bool isStringBegin(char ch) { return ch == '\"'; }

bool isListBegin(char ch) { return ch == '['; }

bool isJsonNestingBegin(char ch) { return ch == '{'; }

bool isBooleanTrue(char ch) { return ch == 't'; }

bool isBooleanFalse(char ch) { return ch == 'f'; }

bool isLiteralNull(char ch) { return ch == 'n'; }

bool isWhiteSpace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

char escapeChar(char ch) {
    switch (ch) {
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case 'r':
            return '\r';
        case 'b':
            return '\b';
        case 'f':
            return '\f';
        case '0':
            return '\0';
        case 'a':
            return '\a';
        case 'v':
            return '\v';
        default:
            return ch;
    }
}

char unescapeChar(char ch) {
    switch (ch) {
        case '\n':
            return 'n';
        case '\t':
            return 't';
        case '\r':
            return 'r';
        case '\b':
            return 'b';
        case '\f':
            return 'f';
        case '\0':
            return '0';
        case '\a':
            return 'a';
        case '\v':
            return 'v';
        default:
            return ch;
    }
}

_TJsonToken::Type scanChar(char json_begin = ' ') {
    /***
     * @description: scan the json string begin
     * and return the token type
     ***/
    if (json_begin == ' ') {
        return _TJsonToken::END;
    }
    else if (isNumberBegin(json_begin)) {
        // a number
        return _TJsonToken::VALUE_NUMBER;
    }
    else if (isStringBegin(json_begin)) {
        // a string
        return _TJsonToken::VALUE_STRING;
    }
    else if (isListBegin(json_begin)) {
        // a list
        return _TJsonToken::LIST_BEGIN;
    }
    else if (isJsonNestingBegin(json_begin)) {
        // a json object
        return _TJsonToken::BEGIN_OBJECT;
    }
    else if (isBooleanTrue(json_begin)) {
        return _TJsonToken::LITERAL_TRUE;
    }
    else if (isBooleanFalse(json_begin)) {
        return _TJsonToken::LITERAL_FALSE;
    }
    else if (isLiteralNull(json_begin)) {
        return _TJsonToken::LITERAL_NULL;
    }
    else {
        switch (json_begin) {
            case '}':
                return _TJsonToken::END_OBJECT;
            case ',':
                return _TJsonToken::VALUE_SEPRATOR;
            case ':':
                return _TJsonToken::NAME_SEPRATOR;
            case ']':
                return _TJsonToken::LIST_END;
            default:
                break;
        }
        return _TJsonToken::END;
    }
}

std::string escapeString(const std::string& str) {
    /***
     * @description: make the string with escape char
     ***/
    std::string res;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\') {
            if (i + 1 < str.size()) {
                res.push_back(escapeChar(str[i + 1]));
                ++i;
            }
        }
        else {
            res.push_back(str[i]);
        }
    }
    return res;
}

// will make the escape char to normal
std::string unescapeString(const std::string& str) {
    auto IsEscapeChar = [](char ch) -> bool {
        return (ch == '\n' || ch == '\t' || ch == '\a' || ch == '\b' ||
                ch == '\t' || ch == '\0' || ch == '\v' || ch == '\f');
    };
    std::string res;
    for (size_t i = 0; i < str.size(); ++i) {
        if (IsEscapeChar(str[i])) {
            res.append(R"(\)");
            res.push_back(unescapeChar(str[i]));
        }
        else {
            res.push_back(str[i]);
        }
    }
    return res;
}

std::size_t jumpWhiteSpace(
  const std::string_view str, std::size_t begin) noexcept {
    for (; begin < str.size(); ++begin) {
        if (!isWhiteSpace(str[begin])) {
            break;
        }
    }
    return begin;
}

template < typename T >
std::optional< T > tryParse(std::string_view str) noexcept {
    T value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    if (res.ec == std::errc() && res.ptr == str.end()) {
        return value;
    }
    return std::nullopt;
}

void update_state(std::string& json_str, _TJsonToken::Type& state,
  std::size_t& reads, std::size_t begin) {
    reads    = jumpWhiteSpace(json_str, begin);
    state    = scanChar(json_str[reads]);
    json_str = json_str.substr(reads);
};

TJsonObj dealValueString(
  std::string& json_str, _TJsonToken::Type& state, std::size_t& reads) {
    // find the end of the json object
    auto find_res = json_str.find("\"");
    if (find_res != std::string::npos) {
        for (std::size_t i = find_res + 1; i < json_str.size(); i++) {
            if (json_str[i] == '\"') {
                find_res = i;
                break;
            }
        }
        // the json object to be implemented
        std::string res = json_str.substr(1, find_res - 1).data();
        json_str        = json_str.substr(find_res + 1);
        return TJsonObj{std::move(res)};
    }
    else {
        throw std::invalid_argument(
          "\033[1;31mjson string end with no \"\033[0m");
    }
}

TJsonObj dealValueNumber(std::string& json_str, _TJsonToken::Type& state,
  std::size_t& reads) { // regex the float number and the scientific notation
    std::regex number_re{"[-]?[0-9]+(\\.[0-9]*)?([eE][+-]?[0-9]+)?"};

    //  store the match result
    std::match_results< decltype(json_str.begin()) > sub_match;

    if (std::regex_search(
          json_str.begin(), json_str.end(), sub_match, number_re))
    {
        // if we find the number, then we try to parse it
        auto str = sub_match.str();
        if (auto value = tryParse< int >(str); value.has_value()) {
            json_str = json_str.substr(str.size());
            return TJsonObj{value.value()};
        }
        if (auto value = tryParse< double >(str); value.has_value()) {
            json_str = json_str.substr(str.size());
            return TJsonObj{value.value()};
        }
    }
    return TJsonObj{};
}

template < typename Callable >
TJsonObj deaList(std::string& json_str, _TJsonToken::Type& state,
  std::size_t& reads, Callable&& op) {
    TJsonObj::ListType res{};
    reads = 1;
    while (reads < json_str.size()) {
        update_state(json_str, state, reads, 1);

        if (json_str[0] == ']' && json_str.size() > 1) {
            state    = _TJsonToken::LIST_END;
            json_str = json_str.substr(1);
            return TJsonObj{std::move(res)};
        }

        auto obj = std::forward< Callable >(op)(json_str, state, reads);

        res.push_back(std::move(obj));
    }
    return TJsonObj{std::move(res)};
}

template < typename Callable >
TJsonObj dealObjBegin(std::string& json_str, _TJsonToken::Type& state,
  std::size_t& reads, Callable&& op) {
    TJsonObj::DictType res{};
    reads = 1;
    while (reads < json_str.size()) {
        update_state(json_str, state, reads, 1);

        if (json_str[0] == '}' && json_str.size() > 1) {
            state    = _TJsonToken::END_OBJECT;
            json_str = json_str.substr(1);
            return TJsonObj{std::move(res)};
        }

        if (json_str[0] == ':') {
            state    = _TJsonToken::VALUE_SEPRATOR;
            json_str = json_str.substr(1);
            return std::forward< Callable >(op)(json_str, state, reads);
        }

        // get the key
        auto keyobj = std::forward< Callable >(op)(json_str, state, reads);

        if (std::holds_alternative< std::string >(keyobj.get())) {
            // to get the value,should make sure the map has the
            // key, then to find the value.
            res[std::get< std::string >(keyobj.get())] =
              TJsonObj{std::monostate{}};
            update_state(json_str, state, reads, 1);
            auto valobj = std::forward< Callable >(op)(json_str, state, reads);
            res[std::get< std::string >(keyobj.get())] = std::move(valobj);
        }
    }
    return TJsonObj{std::move(res)};
}

} // namespace _ParserScan
} // namespace __detail

} // namespace tjson
} // namespace lap

#endif // __TJSON_DETAIL_HPP__
