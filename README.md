
# cpp-tiny-json
+ A Toy Json Parser Implemented by C++20

+ now can only read json file with "{" begin

+ please use g++, MSVC can do well also, but LLVM Clang may not support some features.

## command line args
+ -h, --help: show this help message
+ -r, --read: read which file
+ -s, --store: store to which file
+ -p, --print: print the json object
+ -f, --find: find the key's value in json object

## example

### CLI:

```bash
$ tjson -r ./file.json -s ./file_store.json
$ tjson -r ./file.json -p -f "name" 
```
### Code:

```cpp
#include <tjson.hpp>
#include <tjson/tjfile.hpp>
#include <tjson/tjprint.hpp>


auto main() -> signed {
    using namespace lap::tjson;
    try {
        TJsonFile tjf;
        tjf.readJsonFile("./test.json");

        TJson tj;
        tj.setJsonStr(tjf.getJsonStr());
        tjf.dumpJsonObj2File(tj, "./testDump.json");
        
        // @note: in header file tjson/tjprint.hpp
        Tjprint(tj);

        tj.find("name").println();

        tj["list"][4].println();
        tj["list"][5].println();

        for (auto iter = tj.cbegin(); iter != tj.cend(); ++iter) {
            std::cout << iter->first << ": ";
            iter->second.println();
        }

        tj["student"] = TJsonObj::NestingType{
          {"li",  10},
          {"bai", 9 }
        };
        tj.find("lop") = "hl";
        tj["list"][3]  = "chage] here";
        std::cout << tj << '\n';

        tjf.dumpJsonObj2File(tj, "./testDumpChange.json");

        tj.find("this will throw error");
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
    }
}
```

## question

I find that clang is likely can't compile this project.
It's may be that clang++ doesn't support `std::from_chars` with `<double>` type.
But I use the feature. So hope you can use g++ to compile this project.
