# cpp-tiny-json
A Toy Json Parser Implemented by C++20

now can only read json file with "{" begin

## command line args
-h, --help: show this help message
-r, --read: read which file
-s, --store: store to which file
-p, --print: print the json object
-f, --find: find the key's value in json object

## example

```bash
$ tjson -r ./file.json -s ./file_store.json
$ tjson -r ./file.json -p -f "name" 
```