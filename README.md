# Basic & modern parameter parser for C++ 
Basic file parser to easily pass down parameters to C++ applications.<br>
To be used for example for scientific computations. 
This works nicely in conjunction with [Python Simulation Manager](https://github.com/so-groenen/python_simulation_manager) to control, from a Python notebook, high-performance calculations in C++23.<br> 
This can be a "light-weight" alternative for people (for ex physicist) who prefer working with .txt rather than json or yaml.
TODO: Add DMRG example.<br>

## Features
* Basic utility to "map" text lines into  unordered maps as key-value paris, and parse values as integral/floating point types, as well as `std::vectors` or strings.
* Packaging using CMake for easy installation. Can be used with the CMake `find_package` function.<br>
* Uses modern C++20/23 features: `std::ranges`/`std::views`, concepts, `std::println`, `std::expected` etc...
* Unit tests using GoogleTest & CTest

No exceptions are thrown, only `std::expected<T,Errors>` are returned!  
## Usage

Consider the following `parameters.txt` that we want to parse into `std::vector`s, float, etc:
```
good_vector => 1.0, 1.1, 1.2, 1.3, 1.4, 1.5,  1.6, 1.7,  1.8, 1.9
bad_vector  => 1.0, 1.1, 1.2, XXX, 1.4, YYY, 1.6,  ?!+, 1.8, 1.9
my_str      => hello world
counting   => 1 AND 2 AND 3 AND 4 AND 5 
my_double   => 3.14159265359
the answer  => 42
bad_float  => meow
```
The ists of numbers can be polluted, have uneven spaces etc... <br>
We can parse the text as follows:
```c++
#include "parameter_parser/reader.hpp"
#include "parameter_parser/reader_error.hpp"
 
using parameter_parser::reader::ParameterReader;

int main(int argc, const char* argv []){

    const char* file_name = "parameters.txt";
    auto reader           = ParameterReader::build(file_name, "=>");
    if (!reader){
        std::println("Could not build reader: {}", reader.error());
        return EXIT_FAILURE;
    }

    auto parameters      = reader.value();
    auto good_vec_res    = parameters.try_parse_vector<float>("good_vector", ",");
    auto bad_vec_res     = parameters.try_parse_vector<double>("bad_vector", ",");
    auto double_res      = parameters.try_parse_num<double>("my_double");
    auto bad_float_res   = parameters.try_parse_num<float>("my_bad_float");
    auto hello_world_res = parameters.try_get_str("my_str");

    std::vector good_vec{};
    if(good_vec_res){
        good_vec = std::move(good_vec_res.value());      //OK!!
    }
    else{
        std::println("Error: {}", good_vec_res.error());
    }

    std::vector bad_vec{};
    if(bad_vec){
        bad_vec = std::move(bad_vec_res.value());      //Parsing Error "handled" in else branch:
    }
    else{
        std::println("Error: {}", bad_vec_res.error());
        // output:   "Error: from: "try_parse_vec", kind: "ParseError", args: "XXX, YYY, ?!+"
    }

    
    if(!bad_vec_res)
    {
        std::println("Error: {}", bad_vec_res.error());
    }

    // etc...
}
```
By default, if a line cannot be parsed (*ie* in this example, if they do not contain a `=>`), it will cause the reader to return 
a `ReaderError` whose field `args` contains the line at fault. This can be overriden by setting the reading mode to Permissive: `ParameterReader::build(file_name, "=>", mode::Permissive)`. The line at fault will then simply be ignored.
## Parsing without error checking

Moreover, if all the parameters are needed for a process to run, we can exit the program & print the errors to the console at parsing failures.
This can even be written in a single "functional programming" kind of style:
```c++
    int32_t my_double        = parameters.parse_num_or_exit<double>("my_double");
    uint64_t the_answer      = parameters.parse_num_or_exit<uint64_t>("the answer");
    std::vector count_vec    = parameters.parse_vector_or_exit<int>("counting", "AND");
    float bad_float          = parameters.parse_num_or_exit<float>("bad_float");
    // Will Output: Exiting with ReaderError: from: "try_parse_num", kind: "ParseError", args: "meow"
```

The last line is essentially "syntactic sugar" for.
```c++
    using parameter_parser::reader::exit_if_err;

    std::vector count_vec = parameters.try_parse_vector<int>("counting", "AND").transform_error(exit_if_err).value();
```
## Using the lib

The easiest way is to use CMake's *FetchContent* function. For example, you may add the following lines in your CMake project to link to the library:

```cmake
include(FetchContent)
FetchContent_Declare(
    parameter_parser
    GIT_REPOSITORY https://github.com/so-groenen/parameter_parser_cpp23.git
    GIT_TAG origin/master
)
set(ENABLE_TESTING OFF CACHE INTERNAL "") # Replace with "ON" to enable testing! 
set(ADD_EXAMPLE    OFF CACHE INTERNAL "") # Replace with "ON" to add example! 

FetchContent_MakeAvailable(parameter_parser)

target_link_libraries(${PROJECT_NAME} PRIVATE parameter_parser::parameter_parser)
```
## Tests

Tests testing the basic functionalities using CTest/Google test are also provided. Either Use `-DENABLE_TESTING=ON` when building in the commandline, or use the `set(ENABLE_TESTING ON CACHE INTERNAL "")` just before the call to `FetchContent_MakeAvailable`.