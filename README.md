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

Consider the following `parameters.txt`:
```
good_vector => 1.0,   1.1,  1.2, 1.3, 1.4,     1.5,  1.6, 1.7,  1.8, 1.9
bad_vector  => 1.0, 1.1, 1.2, XXX, 1.4, YYY, 1.6,  ?!+, 1.8, 1.9
my_str      => hello world
my_double   => 3.14159265359
my_bad_float => meow
```
where list of numbers can be polluted, have uneven spaces etc...
We can parse the text as follows:
```c++
#include "parameter_parser/reader.hpp"
#include "parameter_parser/reader_error.hpp"
 
using parameter_parser::reader::ParameterReader;

int main(int argc, const char* argv [])
{
    const char* file_name = "parameters.txt";
    auto reader           = ParameterReader::build(file_name, "=>");
    if (!reader)
    {
        std::println("Could not build reader: {}", reader.error());
        return EXIT_FAILURE;
    }

    auto parameters      = reader.value();
    auto good_vec_res    = parameters.try_parse_vector<float>("good_vector", ",");
    auto bad_vec_res     = parameters.try_parse_vector<float>("bad_vector", ",");
    auto double_res      = parameters.try_parse_num<double>("my_double");
    auto bad_float_res   = parameters.try_parse_num<float>("my_bad_float");
    auto hello_world_res = parameters.try_get_str("my_str");

    if(good_vec_res)
    {
        std::vector good_vec = good_vec_res.value();
    }
    
    if(!bad_vec_res)
    {
        std::println("Error: {}", bad_vec_res.error());
        // output:   "Error: from: "try_parse_vec", kind: "ParseError", args: "XXX, YYY, ?!+"
    }

    // etc...
}
```
By default, if a line cannot be parsed (*ie* in this example, if they do not contain a `=>`), it will cause the reader to return 
a `ReaderError` whose field `args` contains the line at fault. This can be overriden by setting the reading mode to Permissive: `ParameterReader::build(file_name, "=>", mode::Permissive)`. The line at fault will then simply be ignored.

Moreover, if all the parameters are needed for a process to run, we can exit the program & print the errors to the console at parsing failures.
This can even be written in a single "functional programming" kind of style:
```c++
    using parameter_parser::reader::exit_if_err;

    std::vector good_vec = parameters.try_parse_vector<float>("good_vector", ",").transform_error(exit_if_err).value();

```
