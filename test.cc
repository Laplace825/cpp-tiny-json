/**
 * @author: Laplace825
 * @date: 2024-06-28 10:23:40
 * @lastmod: 2024-06-30T00:14:43
 * @description: a test file
 * @filePath: /tiny-json/test.cc
 * @lastEditor: Laplace825
 * @ MIT lisence
 */

#include "tjson.hpp"
#include "tjson/tjfile.hpp"
#include "tjson/tjprint.hpp"
#include "tjson/tjsonParser.hpp"

auto main() -> signed
{
    using namespace lap::tjson;
    std::cout << "can only read json file with '{' begin\n";
    TJsonFile tjf;
    std::string read_path;
    std::cout << "\033[1;32mread which ? \033[0m";
    std::cin >> read_path;
    tjf.readJsonFile(read_path);

    std::cout << "\033[1;32mstore to ? \033[0m";
    std::cin >> read_path;

    TJson tj;
    tj.setJsonStr(tjf.getJsonStr());
    tjf.dumpJsonObj2File(tj, read_path);
}
