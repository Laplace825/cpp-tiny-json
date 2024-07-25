/**
 * @author: Laplace825
 * @date: 2024-06-29T11:23:52
 * @lastmod: 2024-06-30T13:59:50
 * @description: print the json object, you can also use other types, but not
 * recommended
 * @filePath: /tiny-json/header-only/include/tjson/tjprint.hpp
 * @lastEditor: Laplace825
 * @ MIT license
 */

#include "tjson.hpp"
#include "tjson/tjsonObj.hpp"

namespace lap {

namespace tjson {

/**
 * @description: print the json object, you can also use other types, but not
 * recommended
 * @param tj {T}: T may be TJson, TJsonObj
 */
template < typename T >
void TJprint(const T& tj) {
    if constexpr (std::is_same_v< T, TJsonObj >) {
        tj.println();
    }
    else if constexpr (std::is_same_v< T, TJson >) {
        tj.println();
    }
    else {
        std::cout << std::format("{}", tj) << std::endl;
    }
}

} // namespace tjson

} // namespace lap
