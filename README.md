[![C++ CI build](https://github.com/oskarirauta/json_cpp/actions/workflows/build.yml/badge.svg)](https://github.com/oskarirauta/json_cpp/actions/workflows/build.yml)

## json_cpp
Simple C++ JSON library

### About
json_cpp is a lightweight JSON library for exporting data in JSON format from C++.
By taking advantage of templates and operator overloading on the backend, you're able to create and work with JSON objects right away, just as you would
expect from a language such as JavaScript. 
json_cpp does not have any other requirements, such as submodules. Original version was completely in single header
file, but some of code, un-related to templates, has been moved to json.cpp file to make header file smaller. This does
not give any advantage in result, result is the same; but in my opinion, it makes reading the header file easier when it is shortened.

#### Platforms
json_cpp should work on any platform; it's only requirement is a C++17 compatible compiler, as it make heavy use of the C++11 and C++17
move semantics, and variadic templates. The tests are tailored for linux, but could be ported to any platform with relatively new compiler.

### API
You can find the API [over here](API.md). For now it's just a Markdown file with C++ syntax highlighting, but it's better than nothing!

### License
MIT License

#### Original License
Do what the fuck you want public license 

### Usage in project
json_cpp can be used by importing it as a submodule to "json" directory, unless you change
directory name with variable JSON_DIR, check Makefile for example.
Then you just include json/Makefile.inc and link with $(JSON_OBJS) while your code
is including "json.hpp". Utilized variables are CXXFLAGS and INCLUDES, again check
[Makefile](Makefile) for example on library usage when imported.
You can also use different ways, but that needs more manual labour work, though
it's not much.

### Examples
Example code is provided in 'examples' directory. Check out [API](API.md) for more.
I have separated examples to multiple files and you can build a test to see output.

```cpp
#include <iostream>
#include "json.hpp"

int main() {
  JSON obj;
  // Create a new Array as a field of an Object.
  obj["array"] = JSON::Array( true, "Two", 3, 4.0 );
  // Create a new Object as a field of another Object.
  obj["obj"] = JSON::Object();
  // Assign to one of the inner object's fields
  obj["obj"]["inner"] = "Inside";
  
  // We don't need to specify the type of the JSON object:
  obj["new"]["some"]["deep"]["key"] = "Value";
  obj["array2"].append( false, "three" );
  
  // We can also parse a string into a JSON object:
  obj["parsed"] = JSON::Load( "[ { \"Key\" : \"Value\" }, false ]" );
  
  std::cout << obj << std::endl;
}
```

Output:
``` 
{
  "array" : [true, "Two", 3, 4.000000],
  "array2" : [false, "three"],
  "new" : {
    "some" : {
      "deep" : {
        "key" : "Value"
      }
    }
  },
  "obj" : {
    "inner" : "Inside"
  },
  "parsed" : [{
      "Key" : "Value"
    }, false]
}
```

This example can also be written another way:
```cpp
#include <iostream>
#include "json.hpp"

int main() {
    JSON obj = {
        "array", json::Array( true, "Two", 3, 4.0 ),
        "obj", {
            "inner", "Inside"
        },
        "new", { 
            "some", { 
                "deep", { 
                    "key", "Value" 
                } 
            } 
        },
        "array2", json::Array( false, "three" )
    };

    std::cout << obj << std::endl;
```

Sadly, we don't have access to the : character in C++, so we can't use that to seperate key-value pairs, but by using commas,
we can achieve a very similar effect. The other point you might notice, is that we have to explictly create arrays.
This is a limitation of C++'s operator overloading rules, so we can't use the [] operator to define the array.
