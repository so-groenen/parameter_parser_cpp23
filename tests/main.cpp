#include <gtest/gtest.h>
#include "parameter_parser/reader.hpp"
#include "parameter_parser/utilities.hpp"
#include "parameter_parser/reader_error.hpp"

using namespace std::string_view_literals;
using namespace parameter_parser;
using reader::ParameterReader;

template <typename T, typename Err>
using Result = std::expected<T,Err>;
 
template <typename T>
using Vec    = std::vector<T>;
 

const char* file_name = "parameters.txt";

TEST(UtilitiesTest, trim) 
{
    std::string hello = "    hello world     ";
    EXPECT_EQ(utilities::trim(hello), "hello world"sv);
}

TEST(UtilitiesTest, VectorParsingInts) 
{
    std::string int_list  = " 1 and 2  and 3and    4and  5 and6     ";
    auto expected_int_vec = Vec<int>{1, 2, 3, 4, 5, 6};         
    auto int_vec_result   = utilities::try_parse_vec<int>(int_list, "and");

    ASSERT_TRUE(int_vec_result.has_value());
    EXPECT_EQ(int_vec_result.value(), expected_int_vec);
}

TEST(UtilitiesTest, VectorParsingDouble) 
{
    std::string double_list = " 1.42 et 2.42  et 3.42et    4.42et  5.42 et6.42     ";
    auto expected_vec       = Vec<double>{1.42, 2.42, 3.42, 4.42, 5.42, 6.42};         
    auto vec_result         = utilities::try_parse_vec<double>(double_list, "et");

    ASSERT_TRUE(vec_result.has_value());
    EXPECT_EQ(vec_result.value(), expected_vec);
}

TEST(UtilitiesTest, VectorParsingErrorHandled) 
{
    std::string bad_list = " 1.42 et 2.42  et 3.42et   Meow et  Meow! et6.42     ";
    auto bad_vec_result  = utilities::try_parse_vec<double>(bad_list, "et");

    ASSERT_FALSE(bad_vec_result.has_value());
    EXPECT_EQ(bad_vec_result.error(), "Meow, Meow!");
}

class ReaderTest : public testing::Test
{
protected:
    Result<reader::ParameterReader,ReaderError> reader_result;
    std::unique_ptr<reader::ParameterReader> reader{};

    Result<Vec<float>,ReaderError> good_vec_res;
    Result<Vec<float>,ReaderError> bad_vec_res;
    Result<Vec<float>,ReaderError> not_exist_res;
    Result<double,ReaderError> my_double_res;   
    Result<int,ReaderError> my_int_res;      
    Result<float,ReaderError> none_float;     
    Result<float,ReaderError> my_bad_float;   
    Result<std::string,ReaderError> hello_world_res;
    Result<std::string,ReaderError> none_str;

    ReaderTest() 
        : reader_result{reader::ParameterReader::build(file_name, "=>")}
    {
        if (reader_result)
        {
            reader          = std::make_unique<reader::ParameterReader>(std::move(reader_result.value()));

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
}

TEST_F(ReaderTest, ParsingVectorsErrorsHandled) 
{
    ASSERT_FALSE(bad_vec_res.has_value())   << "bad_vec_res should have parse error";
    ASSERT_FALSE(not_exist_res.has_value()) << "not_exist_res should not exist";

    EXPECT_EQ(bad_vec_res.error().args, "XXX, YYY, ?!+");
    EXPECT_EQ(bad_vec_res.error().kind, ReaderError::Kind::ParseError);
    EXPECT_EQ(bad_vec_res.error().from, ReaderError::From::try_parse_vec);

    EXPECT_EQ(not_exist_res.error().args, "non-existing vector");
    EXPECT_EQ(not_exist_res.error().kind, ReaderError::Kind::KeyError);
    EXPECT_EQ(not_exist_res.error().from, ReaderError::From::try_parse_vec);
}

TEST_F(ReaderTest, ParsingVectorsExitHandled) 
{
    // std::string s1  = "Exiting with ReaderError: from: .*, kind: .*, args: .*";
    // const auto& s1_ = s1;
    EXPECT_DEATH(reader->parse_vector_or_exit<float>("non-existing vector", ","),
                "Exiting with ReaderError: from: .*, kind: .*, args: .*");

    // std::string s2  = "Exiting with ReaderError: from: \"try_parse_vec\", kind: \"ParseError\", args: \"bad_vector\"";
    // const auto& s2_ = s2;
    EXPECT_DEATH(reader->parse_vector_or_exit<float>("bad_vector", ","),
                "Exiting with ReaderError: from: .*, kind: .*, args: .*");
}

TEST_F(ReaderTest, ParsingNums) 
{
    EXPECT_TRUE(my_double_res.has_value());
    EXPECT_EQ(my_double_res.value(), 3.14159265359);

    EXPECT_TRUE(my_int_res.has_value());
    EXPECT_EQ(my_int_res.value(), 42);
}

TEST_F(ReaderTest, ParsingNumsErrorsHandled) 
{  
    ASSERT_FALSE(none_float.has_value())  << "No float should be here!";
    ASSERT_FALSE(my_bad_float.has_value()) << "No float should be here!";

    EXPECT_EQ(none_float.error().from, ReaderError::From::try_parse_num);
    EXPECT_EQ(none_float.error().kind, ReaderError::Kind::KeyError);
    EXPECT_EQ(none_float.error().args, "non-existing float");

    EXPECT_EQ(my_bad_float.error().from, ReaderError::From::try_parse_num);
    EXPECT_EQ(my_bad_float.error().kind, ReaderError::Kind::ParseError);
    EXPECT_EQ(my_bad_float.error().args, "meow");
}

TEST_F(ReaderTest, ParsingNumsExitHandled) 
{  

    EXPECT_DEATH(reader->parse_num_or_exit<float>("non-existing float"),
                "Exiting with ReaderError: from: .*, kind: .*, args: .*");

    EXPECT_DEATH(reader->parse_num_or_exit<float>("my_bad_float"),
                "Exiting with ReaderError: from: .*, kind: .*, args: .*");
}

TEST_F(ReaderTest, StringTests) 
{
    EXPECT_TRUE(hello_world_res.has_value());
    EXPECT_EQ(hello_world_res.value(), "hello world");
}

TEST_F(ReaderTest, StringTestsErrorHandled) 
{
    ASSERT_FALSE(none_str.has_value()) << "No string should be here!";

    EXPECT_EQ(none_str.error().from, ReaderError::From::try_parse_str);
    EXPECT_EQ(none_str.error().kind, ReaderError::Kind::KeyError);
    EXPECT_EQ(none_str.error().args, "non-existing str");
}

TEST_F(ReaderTest, StringExitHandled) 
{  
    EXPECT_DEATH(reader->get_str_or_exit("non-existing str"),
                "Exiting with ReaderError: from: .*, kind: .*, args: .*");
}