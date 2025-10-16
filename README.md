### WORK IN PROGRESS
# Basic raw text parser for modern C++
Basic raw text parser to easily parse parameters in C++ applications.<br>
To be used for example in scientific computations. 
This works nicely in conjunction with [Python Simulation Manager](https://github.com/so-groenen/python_simulation_manager) to control, from a Python notebook, scientific calculations and simulations in modern C++. 
This can be a "light-weight" alternative for people working in fields where using raw text formats 
over json is the norm (for example theoretical physics).<br>
### Requires
* C++23 compiler with support for `std::ranges`/`std::expected`/concepts.
## Features
* Basic utility to map raw text lines into unordered maps as key-value pairs
* Parses int/floating point types, as well as `std::vector`.
* Packaging using CMake for easy installation. Can be used with the CMake `find_package` function.<br>
* Uses modern C++20/23 features like `std::ranges`/`std::views`, concepts, `std::println`, `std::expected` etc...
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
The lists of numbers can also have uneven spaces. <br>
We can parse the text as follows:
```c++
#include "parameter_parser/reader.hpp"
#include "parameter_parser/reader_error.hpp"
 
using parameter_parser::reader::ParameterReader;

int main(int argc, const char* argv []){

    std::string_view file_name{"parameters.txt"};
    
    auto reader = ParameterReader::build(file_name, "=>");
    if (!reader){
        std::println("Could not build reader: {}", reader.error());
        return EXIT_FAILURE;
    }

    auto parameters      = reader.value();
    auto good_vec_res    = parameters.try_parse_vector<float>("good_vector", ",");
    auto bad_vec_res     = parameters.try_parse_vector<float>("bad_vector", ",");
    auto double_res      = parameters.try_parse_num<double>("my_double");
    auto bad_float_res   = parameters.try_parse_num<float>("my_bad_float");
    auto hello_world_res = parameters.try_get_str("my_str");

    std::vector<float> good_vec{};
    if(good_vec_res){
        good_vec = std::move(good_vec_res.value());      //OK!!
    }
    else{
        std::println("Error: {}", good_vec_res.error());
    }

    std::vector<float> bad_vec{};
    if(bad_vec){
        bad_vec = std::move(bad_vec_res.value());       //Parsing Error "handled" in "else" branch:
    }
    else{                                               //output:
        std::println("Error: {}", bad_vec_res.error()); //"Error: from: "try_parse_vec", kind: "ParseError", args: "XXX, YYY, ?!+"
    }

    double my_double = double_res.value_or(0.0);         // Ok: 3.14159265359
    double my_float  = bad_float_res.value_or(0.0f);     // Error: will output 0
    // etc...
}
```
By default, if a line cannot be parsed (*ie* in this example, if a line does not contain a `=>`), it will cause the reader to return 
a `ReaderError` whose field `args` contains the line at fault. This can be overriden by setting the reading mode to Permissive:
```c++
auto reader = ParameterReader::build(file_name, "=>");
```
The line at fault will then simply be ignored.<br>
More "functional programming" techniques to handle the results/errors can be found here: [in this article](https://www.cppstories.com/2024/expected-cpp23-monadic/).

## Parse or exit

Often times in scientific computations, all parameters are crucial, and the process can no longer continue if a single one is missing.<br>
The `parse_or_exit` methods does exactly that: parse, otherwise exits the program & output the culprit to the console:
```c++
    int32_t my_double        = parameters.parse_num_or_exit<double>("my_double");
    uint64_t the_answer      = parameters.parse_num_or_exit<uint64_t>("the answer");
    std::vector count_vec    = parameters.parse_vector_or_exit<int>("counting", "AND");
    float bad_float          = parameters.parse_num_or_exit<float>("bad_float");
    //output: Exiting with ParameterParserError: from: "parse_num_or_exit", kind: "ParseError", args: "meow"
```
## Using the lib

The easiest way is to use CMake's *FetchContent* function. For example, you may add the following lines to your CMake project to link to the library:

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

The basic functionalities are "unit tested" using CTest/Google tests which are provided. Either Use `-DENABLE_TESTING=ON` when building in the commandline, or use the `set(ENABLE_TESTING ON CACHE INTERNAL "")` just before the call to `FetchContent_MakeAvailable`.
