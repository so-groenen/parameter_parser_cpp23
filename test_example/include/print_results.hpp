#ifndef HELPER_HPP
#define HELPER_HPP

#include <print>


void print_vector_result(const auto& vec_res)
{
    if(!vec_res)
    {
        std::println("ERROR: {}", vec_res.error());
        return;
    }
    for (const auto& v : vec_res.value())
    {
        std::println("v: {}", v);
    }
}
void print_result(const auto& res)
{
    if(!res)
    {
        std::println("ERROR: {}", res.error());
        return;
    }
    std::println("VALUE: {}", res.value());
}

#endif
