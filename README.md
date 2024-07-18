
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

```bash
$ tjson -r ./file.json -s ./file_store.json
$ tjson -r ./file.json -p -f "name" 
```

## question

I find that clang is likely can't compile this project.
It's may be that clang++ doesn't support `std::from_chars` with `<double>` type.
But I use the feature. So hope you can use g++ to compile this project.
