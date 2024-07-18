/**
 * @author: Laplace825
 * @date: 2024-06-28 10:23:40
 * @lastmod: 2024-07-19T01:59:44
 * @description: a test file
 * @filePath: /cpp-tiny-json/test.cc
 * @lastEditor: Laplace825
 * @ MIT lisence
 */

#include "tjson.hpp"
#include "tjson/tjfile.hpp"
#include "tjson/tjprint.hpp"
#include "tjson/tjsonObj.hpp"
#include "tjson/tjsonParser.hpp"

auto main() -> signed {
    using namespace lap::tjson;
    TJsonFile tjf;
    tjf.readJsonFile("./test.json");

    std::cout << "\033[1;32mstore to ./testDump.json \033[0m";

    TJson tj;
    tj.setJsonStr(tjf.getJsonStr());
    tjf.dumpJsonObj2File(tj, "./testDump.json");

    std::cout << "\033[1;32mprint the json object\033[0m\n";
    Tjprint(tj);
    std::cout << "\033[1;32mfn find the key's value in json object\033[0m\n";
    tj.find("name")->println();
    std::cout << "\033[1;32mop [] the key's value in json object\033[0m\n";
    (*tj["list"])[4].println();
    (*tj["list"])[5].println();

    *tj["student"] = 100;
    tj.println();
    tjf.dumpJsonObj2File(tj, "./testDumpChangeStudent.json");
}
