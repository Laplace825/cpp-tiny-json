/**
 * @author: Laplace825
 * @date: 2024-06-28 10:23:40
 * @lastmod: 2024-07-14T23:58:24
 * @description: the main file to deal command line args
 * @filePath: /tiny-json/main.cc
 * @lastEditor: Laplace825
 * @ MIT license
 */

#include <format>
#include <iostream>
#include <string_view>

#include "tjson.hpp"
#include "tjson/tjfile.hpp"
#include "tjson/tjprint.hpp"

using namespace lap::tjson;

TJsonFile tjfile;
TJson tjson;

void help_msg() {
    std::cout << "\033[1;32mcan only read json file with '{' begin\033[0m\n"
                 "[command line args]:\n"
                 "  -h, --help: show this help message\n"
                 "  -r, --read: read which file\n"
                 "  -s, --store: store to which file\n"
                 "  -p, --print: print the json object\n"
                 "  -f, --find: find the key's value in json object\n"
                 "[example]:\n"
                 "tjson -r ./file.json\n -s ./file_store.json\n";
    std::cout << std::format("Now in path \033[1;32m{}\033[0m\n",
      std::filesystem::current_path().c_str());
}

void what_to_do(
  std::string_view command, std::string_view readStoreOrFind = "") {
    if (command == "-h" || command == "--help") {
        help_msg();
    }
    else if (command == "-r" || command == "--read") {
        tjson.clear();
        tjfile.readJsonFile(readStoreOrFind);
        tjson.setJsonStr(tjfile.getJsonStr());
    }
    else if (command == "-s" || command == "--store") {
        tjfile.dumpJsonObj2File(tjson, readStoreOrFind);
    }
    else if (command == "-p" || command == "--print") {
        tjson.println();
    }
    else if (command == "-f" || command == "--find") {
        auto findResult = tjson[readStoreOrFind];
        if (findResult != TJsonObj{}) {
            std::cout << std::format("\033[1;32m{}\033[0m:", readStoreOrFind)
                      << findResult << std::endl;
        }
        else {
            std::cout << "null, \033[1;34mmaybe you should read json file "
                         "first\033[0m\n"
                         "[example] tjson -r ./file.json -f \"lap\"\n";
        }
    }
    else {
        help_msg();
    }
}

auto main(int argc, char* argv[]) -> signed {
    if (argc == 1) {
        help_msg();
        return 0;
    }
    std::vector< std::string_view > args(
      argv + 1, argv + argc); // store all the args

    for (size_t i = 0; i < args.size();) {
        if (args[i] == "-h" || args[i] == "--help") {
            what_to_do(args[i++]);
        }
        if (args[i] == "-p" || args[i] == "--print") {
            what_to_do(args[i++]);
        }
        if (args[i] == "-f" || args[i] == "--find") {
            what_to_do(args[i], args[i + 1]);
            i += 2;
        }
        if (args[i] == "-r" || args[i] == "--read") {
            what_to_do(args[i], args[i + 1]);
            i += 2;
        }
        if (args[i] == "-s" || args[i] == "--store") {
            what_to_do(args[i], args[i + 1]);
            i += 2;
        }
    }
    return 0;
}
