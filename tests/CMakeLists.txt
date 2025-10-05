cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(parameter_parser_tests main.cpp)

target_link_libraries     (parameter_parser_tests GTest::gtest_main parameter_parser::reader parameter_parser::utilities)
target_include_directories(parameter_parser_tests PRIVATE include)
target_compile_features   (parameter_parser_tests PRIVATE cxx_std_23)

add_custom_command(
        TARGET parameter_parser_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/parameters.txt
                ${CMAKE_CURRENT_BINARY_DIR}/parameters.txt)


include(GoogleTest)
gtest_discover_tests(parameter_parser_tests)