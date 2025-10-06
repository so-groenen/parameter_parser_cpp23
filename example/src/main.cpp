#include <iostream>
#include <print>
#include "parameter_parser/reader.hpp"
#include "parameter_parser/reader_error.hpp"

#include <assert.h>
#include "print_results.hpp"
using namespace std::string_view_literals;

#ifdef NDEBUG
#undef NDEBUG
#define DEBUG
#endif


 
int main(int argc, const char* argv [])
{
    using namespace parameter_parser::reader;
    using namespace parameter_parser;
    const char* file_name = "parameters.txt";
    auto reader           = ParameterReader::build(file_name, "=>");
    if (!reader)
    {
        std::println("Could not build reader: {}", reader.error());
        return EXIT_FAILURE;
    }
    assert(reader.has_value());
    std::println("-> assertions passed");
    auto parameters      = reader.value();

    // Parse without error check, will call exit(EXIT_FAILURE) upon error:
    int32_t my_int           = parameters.parse_num_or_exit<int32_t>("my_int");
    std::vector int64_vector = parameters.parse_vector_or_exit<int64_t>("int64_vector", "and");
    std::string hello_world  = parameters.get_str_or_exit("my_str");

    // Also using std::expected & error handling:
    auto good_vec_res    = parameters.try_parse_vector<float>("good_vector", ",");
    auto bad_vec_res     = parameters.try_parse_vector<float>("bad_vector", ",");
    auto not_exist_res   = parameters.try_parse_vector<float>("non-existing vector", ",");
    auto my_double_res   = parameters.try_parse_num<double>("my_double");
    auto none_float      = parameters.try_parse_num<float>("non-existing float");
    auto my_bad_float    = parameters.try_parse_num<float>("my_bad_float");
    auto none_str        = parameters.try_get_str("non-existing str");
    


    std::println("*** Parsing vecs:");
        assert(good_vec_res.has_value());
        auto vec = std::vector{1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f, 1.7f, 1.8f, 1.9f};
        assert(good_vec_res.value() == vec);
        print_vector_result(good_vec_res);

    std::println("Expected errors:");
        assert(!bad_vec_res.has_value());
        assert(!not_exist_res.has_value());
        print_vector_result(bad_vec_res);
        print_vector_result(not_exist_res);

    std::println("*** Parsing nums:");
        assert(my_double_res.has_value());
        assert(my_double_res.value() == 3.14159265359);
        print_result(my_double_res);

    std::println("Expected errors:");
        assert(!none_float.has_value());
        assert(!my_bad_float.has_value());
        print_result(none_float);
        print_result(my_bad_float);

    std::println("*** Parsing strings:");
        assert(!none_str.has_value());

    std::println("Expected errors:");
        print_result(none_str);
}