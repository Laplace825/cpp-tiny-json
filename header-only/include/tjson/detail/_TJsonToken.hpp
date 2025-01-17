/**
 * @author: Laplace825
 * @date: 2024-06-28T10:27:35
 * @lastmod: 2024-07-19T20:28:03
 * @description:
 * @filePath: /cpp-tiny-json/header-only/include/tjson/detail/_TJsonToken.hpp
 * @lastEditor: Laplace825
 * @ MIT license
 */

#ifndef __TJSON_TOKEN_HPP__
#define __TJSON_TOKEN_HPP__

namespace lap {
namespace tjson {

namespace __detail {
struct _TJsonToken {
    using Type = enum {
        BEGIN_OBJECT, // {
        END_OBJECT,   // }

        VALUE_SEPRATOR, // ,
        NAME_SEPRATOR,  // :

        VALUE_STRING, // "string"
        VALUE_NUMBER, // 1, -1e10

        LITERAL_TRUE,  // true
        LITERAL_FALSE, // false
        LITERAL_NULL,  // null

        LIST_BEGIN, // [
        LIST_END,   // ]

        VALUE_JSON_ELEMENT, // JSON nesting

        END // EOF
    };

    Type m_value;

    _TJsonToken(Type value) : m_value(value) {}
};

} // namespace __detail
} // namespace tjson
} // namespace lap

#endif // __TJSON_TOKEN_HPP__
