cmake_minimum_required(VERSION 3.10.0)
project(test_example VERSION 0.1.0 LANGUAGES C CXX)

add_executable(test_example src/main.cpp)
  
add_custom_command(
        TARGET test_example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/parameters.txt
                $<TARGET_FILE_DIR:test_example>/parameters.txt)

target_include_directories(test_example PRIVATE include)
target_link_libraries     (test_example PRIVATE parameter_parser::utilities parameter_parser::reader)
target_compile_features   (test_example PRIVATE cxx_std_23)

