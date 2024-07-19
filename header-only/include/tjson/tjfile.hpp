/**
 * @author: "laplace"
 * @date: 2024-06-29T15:28:39
 * @lastmod: 2024-07-19T20:27:42
 * @description: used to read and store json file
 * @filePath: /cpp-tiny-json/header-only/include/tjson/tjfile.hpp
 * @lastEditor: Laplace825
 * @ MIT license
 */

#ifndef __TJSON_FILE_HPP__
#define __TJSON_FILE_HPP__

#include <filesystem>
#include <fstream>
#include <ios>
#include <type_traits>

#include "detail/_ParserScan.hpp"
#include "tjson.hpp"
#include "tjson/tjsonObj.hpp"

namespace lap {

namespace tjson {

// used to read and store json file
class TJsonFile {
  private:
    std::filesystem::path m_path;
    std::string m_json_str;

  protected:
  public:
    TJsonFile() : m_path(std::filesystem::current_path()) {}

    TJsonFile(const std::filesystem::path& path) : m_path(path) {}

    bool readJsonFile() { return readJsonFile(m_path); }

    bool readJsonFile(const std::filesystem::path& path) {
        std::ifstream ifs(path, std::ios::in);
        std::cout << std::format(
          "\033[1;33mreading\033[0m: {}\n", path.string());
        if (ifs.is_open()) {
            std::string line;
            std::string reads;
            while (std::getline(ifs, line)) {
                reads.append(line);
            }
            m_json_str = std::move(reads);
            ifs.close();
            return true;
        }
        throw std::runtime_error("\033[1;31mfile not found\033[0m");
    }

    std::string getJsonStr() const { return m_json_str; }

    bool storeJsonStr2Where(const std::filesystem::path& path) const {
        std::ofstream ofs("", std::ios::out | std::ios::trunc);
        if (path.string().ends_with(".json")) {
            ofs.open(path);
            std::cout << std::format(
              "\033[1;33mstore to\033[0m : {}\n", path.c_str());
        }
        else {
            auto default_path = path.string() + "/tjson.json";
            ofs.open(default_path);
            std::cout << std::format(
              "\033[1;33mstore to\033[0m : {}\n", std::move(default_path));
        }

        ofs << __detail::_ParserScan::unescapeString(m_json_str);

        if (!ofs) {
            throw std::runtime_error(std::format(
              "\033[1;31m{}\033[0m", "Failed to write file: " + path.string()));
        }
        return true;
    }

    bool storeJsonStr2Where() const { return storeJsonStr2Where(m_path); }

    template < typename T >
        requires(std::is_same_v< T, TJsonObj > || std::is_same_v< T, TJson >)
    bool dumpJsonObj2File(
      const T& json_obj, const std::filesystem::path& path = "") {
        m_json_str = std::move(json_obj.toString());
        return *path.c_str() == '\0' ? storeJsonStr2Where()
                                     : storeJsonStr2Where(path);
    }
};

} // namespace tjson

} // namespace lap

#endif // __TJSON_FILE_HPP__
