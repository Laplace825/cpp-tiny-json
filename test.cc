/**
 * @author: Laplace825
 * @date: 2024-06-28T10:23:40
 * @lastmod: 2024-07-19T20:26:41
 * @description: a test file
 * @filePath: /cpp-tiny-json/test.cc
 * @lastEditor: Laplace825
 * @ MIT lisence
 */

#include <tjson.hpp>
#include <tjson/tjfile.hpp>
#include <tjson/tjprint.hpp>

auto main() -> signed {
    using namespace lap::tjson;
    try {
        TJsonFile tjf;
        std::cout << "\033[1;32m>>> read the json file\033[0m\n";
        tjf.readJsonFile("./test.json");

        TJson tj;
        std::cout
          << "\033[1;32m>>> set the json string to json object\033[0m\n";
        tj.setJsonStr(tjf.getJsonStr());
        tjf.dumpJsonObj2File(tj, "./testDump.json");

        std::cout << "\033[1;32m>>> print the json object\033[0m\n";
        Tjprint(tj);

        std::cout
          << "\033[1;32m>>> fn find the key's value in json object\033[0m\n";
        tj.find("name").println();

        std::cout
          << "\033[1;32m>>> op [] the key's value in json object\033[0m\n";
        tj["list"][4].println();
        tj["list"][5].println();

        std::cout
          << "\033[1;32m>>> use iterator to visit the json object\033[0m\n";
        for (auto iter = tj.cbegin(); iter != tj.cend(); ++iter) {
            std::cout << iter->first << ": ";
            iter->second.println();
        }

        std::cout << "\033[1;32m>>> change the json object\033[0m\n";
        std::cout << "\033[1;32mmake student to a dict\n"
                     "make \"lop\" to \"hl\"\033[0m\n"
                     "make list[3] to \"chage] here\"\n";
        tj["student"] = TJsonObj::DictType{
          {"li",  10},
          {"bai", 9 }
        };
        tj.find("lop") = "hl";
        tj["list"][3]  = "chage] here";
        std::cout << tj << '\n';
        tjf.dumpJsonObj2File(tj, "./testDumpChange.json");

        std::cout
          << "\033[1;32mfind a can't find key in the json object\033[0m\n";
        tj.find("this will throw error");
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
    }
}
