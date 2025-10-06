cmake_minimum_required(VERSION 3.10.0)
project(example VERSION 0.1.0 LANGUAGES C CXX)

add_executable(example src/main.cpp)
  
add_custom_command(
        TARGET example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/parameters.txt
                $<TARGET_FILE_DIR:example>/parameters.txt)

target_include_directories(example PRIVATE include)
target_link_libraries     (example PRIVATE parameter_parser::utilities parameter_parser::reader)
target_compile_features   (example PRIVATE cxx_std_23)

