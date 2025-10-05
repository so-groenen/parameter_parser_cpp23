#include <gtest/gtest.h>
#include "parameter_parser/reader.hpp"
#include "parameter_parser/utilities.hpp"

using namespace std::string_view_literals;
using namespace parameter_parser;
using reader::ParameterReader;

template <typename T, typename Err>
using Result = std::expected<T,Err>;
 
template <typename T>
using Vec    = std::vector<T>;
using String = std::string;


const char* file_name = "parameters.txt";

TEST(UtilitiesTest, trim) 
{
    std::string hello = "    hello world     ";
    EXPECT_EQ(utilities::trim(hello), "hello world"sv);
}

TEST(UtilitiesTest, VectorParsing) 
{
    String list       = " 1 and 2  and 3and    4and  5 and6     ";
    auto expected_vec = std::vector{1,2,3,4,5,6};         
    auto vec_result   = utilities::try_parse_vec<int>(list, "and");

    ASSERT_TRUE(vec_result.has_value());
    EXPECT_EQ(vec_result.value(), expected_vec);
}

class ReaderTest : public testing::Test
{
protected:
    Result<reader::ParameterReader,String> reader_result;
    std::unique_ptr<reader::ParameterReader> reader{};

    Result<Vec<float>,String> good_vec_res;
    Result<Vec<float>,String> bad_vec_res;
    Result<Vec<float>,String> not_exist_res;
    Result<double,String> my_double_res;   
    Result<int,String> my_int_res;      
    Result<float,String> none_float;     
    Result<float,String> my_bad_float;   
    Result<String,String> hello_world_res;
    Result<String,String> none_str;

    ReaderTest() 
        : reader_result{reader::ParameterReader::build(file_name, "=>")}
    {
        if (reader_result)
        {
            reader          = std::make_unique<reader::ParameterReader>(reader_result.value());

            good_vec_res    = reader->try_parse_vector<float>("good_vector", ",");
            bad_vec_res     = reader->try_parse_vector<float>("bad_vector", ",");
            not_exist_res   = reader->try_parse_vector<float>("non-existing vector", ",");
            my_double_res   = reader->try_parse_num<double>("my_double");
            my_int_res      = reader->try_parse_num<int>("my_int");
            none_float      = reader->try_parse_num<float>("non-existing float");
            my_bad_float    = reader->try_parse_num<float>("my_bad_float");
            hello_world_res = reader->try_get_str("my_str");
            none_str        = reader->try_get_str("non-existing str");
        }
    }
};

TEST_F(ReaderTest, ReaderExists) 
{
    EXPECT_TRUE(reader_result.has_value());
}

TEST_F(ReaderTest, ParsingVectors) 
{
    EXPECT_TRUE(good_vec_res.has_value());
    
    auto expected_vec = std::vector{1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f, 1.7f, 1.8f, 1.9f};
    EXPECT_EQ(good_vec_res.value(), expected_vec);

    EXPECT_FALSE(bad_vec_res.has_value())   << "bad_vec_res should have parse error";
    EXPECT_FALSE(not_exist_res.has_value()) << "not_exist_res should not exist";
}

TEST_F(ReaderTest, ParsingNums) 
{
    EXPECT_TRUE(my_double_res.has_value());
    EXPECT_EQ(my_double_res.value(), 3.141);

    EXPECT_TRUE(my_int_res.has_value());
    EXPECT_EQ(my_int_res.value(), 42);
    
    EXPECT_FALSE(none_float.has_value()) << "No float should be here!";
    EXPECT_FALSE(my_bad_float.has_value()) << "No float should be here!";
}

TEST_F(ReaderTest, StringTests) 
{
    EXPECT_TRUE(hello_world_res.has_value());
    EXPECT_EQ(hello_world_res.value(), "hello world");

    EXPECT_FALSE(none_str.has_value()) << "No string should not be here!";
}
 