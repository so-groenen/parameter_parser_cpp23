#ifndef PARSING_UTILITIES_HPP
#define PARSING_UTILITIES_HPP

#include <iostream>
#include <ranges>
#include <vector>
#include <string_view>
#include <expected>
#include <optional>
#include <charconv>

using std::tuple;
using std::optional;
using std::expected;
using str_v = std::string_view;
using namespace std::string_view_literals;
 
namespace parameter_parser::utilities
{
    auto split_once(str_v input, str_v delim) -> optional<tuple<str_v,str_v>>
    {
        size_t n = input.find(delim);
        if (n == str_v::npos)
        {
            return {};
        }
        str_v token1 = input.substr(0, n);
        str_v token2 = input.substr(n + delim.length(), input.length());
        return tuple{token1, token2};
    }

    auto trim(str_v input) -> str_v
    {
        str_v s = input;
        if (size_t front_space = std::min(s.find_first_not_of(' '), s.length())) // if the str_v is nothing but blank we remove everything
        {
            s.remove_prefix(front_space);
        }
        if (size_t last = s.find_last_not_of(' '); last != str_v::npos)
        {
            s = s.substr(0, last+1);
        }
        return s;
    }

    template <typename T>
    auto parse_num_handled(str_v view) -> expected<T, str_v> 
        requires std::is_integral_v<T> || std::is_floating_point_v<T>
    {
        const char* first = view.data();
        const char* last  = view.data() + view.length();
        T value{};
        std::from_chars_result res = std::from_chars(first, last, value);
        if (res.ec == std::errc::invalid_argument)
        {
            return std::unexpected(view);
        }
        return value;
    }


    template <typename T>
    auto try_parse_vec(str_v line, str_v delim) -> expected<std::vector<T>, std::string> 
        requires std::is_integral_v<T> || std::is_floating_point_v<T>
    {
        auto to_str_v  = [](auto token){return str_v{token};};
        auto try_parse = [](auto sv){return parse_num_handled<T>(sv);};
        auto has_error = [](const auto& res){return !res.has_value();};
        auto get_error = [](const auto& res){return res.error();};
        auto get_value = [](const auto& res){return res.value();};

        auto tokens        = line | std::views::split(delim) | std::views::transform(to_str_v);
        auto expected_nums = tokens | std::views::transform(trim) | std::views::transform(try_parse); 
        auto parse_errors  = expected_nums | std::views::filter(has_error) | std::views::transform(get_error); 
        
        if(!std::ranges::empty(parse_errors))
        {
            auto errors_str = parse_errors | std::views::join_with(", "sv) | std::ranges::to<std::string>();
            return std::unexpected(errors_str);
        }
        auto nums = expected_nums | std::views::transform(get_value)| std::ranges::to<std::vector>();
        return nums;
    }
}

#endif