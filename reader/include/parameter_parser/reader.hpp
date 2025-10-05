#ifndef PARAMETER_READER_HPP
#define PARAMETER_READER_HPP

#include <iostream>
#include <istream>
#include <fstream>
#include <unordered_map>
#include <ranges>
#include <print>
#include <string>
#include <vector>
#include <string_view>
#include <expected>
#include "parameter_parser/utilities.hpp"

using str_v = std::string_view;
using ParameterMap = std::unordered_map<std::string, std::string>;

namespace parameter_parser::reader
{
    using utilities::split_once;
    using utilities::trim;
    using utilities::parse_num_handled;
    using utilities::try_parse_vec;
    using namespace std::string_view_literals;


    auto exit_if_err(std::string&& error) -> std::string 
    {
        std::println(">> Exiting with ParameterReaderError: {}", error);
        exit(EXIT_FAILURE);
        return error;
    }

    class ParameterReader
    {
        ParameterMap m_map{};
        std::string m_buffer{};
        ParameterReader(ParameterMap&& map)
            : m_map{map} // call the unordered_map move constructor
        {   
        }
    public:
        enum Mode
        {
            Strict,
            Permissive
        };
    private:
        static auto build_from_ifstream(std::ifstream& parameter_file, str_v delimiter, Mode mode = Strict) -> std::expected<ParameterReader, std::string>
        {
            if (!parameter_file.is_open())
            {
                return std::unexpected(">> ParameterError::FileError Could not open file\n");  
            }

            std::unordered_map<std::string, std::string> parameter_map{};
            std::string buffer{};
            while (std::getline(parameter_file, buffer))
            {
                auto key_val = split_once(buffer, delimiter);
                if (!key_val.has_value() && mode == Strict)
                {
                    return std::unexpected(std::format(">> ParameterError::ParsingFail at line \"{}\"", buffer));
                }
                else if (key_val.has_value())
                {
                    auto[key, value] = key_val.value();
                    parameter_map.emplace(trim(key), trim(value));
                }
            }
            return ParameterReader{std::move(parameter_map)};
        }

    public:
        static auto build(const char* file_path, str_v delimiter, Mode mode = Strict) -> std::expected<ParameterReader, std::string>
        {
            std::ifstream parameter_file{file_path};
            return build_from_ifstream(parameter_file,delimiter, mode);
        }
        static auto build(const std::string& file_path, str_v delimiter, Mode mode = Strict) -> std::expected<ParameterReader, std::string>
        {
            std::ifstream parameter_file{file_path};
            return build_from_ifstream(parameter_file,delimiter, mode);
        }


        template<typename T>
        auto try_parse_num(str_v key) -> std::expected<T, std::string>
            requires std::is_integral_v<T> || std::is_floating_point_v<T>
        {
            m_buffer = key;
            if (!m_map.contains(m_buffer))
            {
                return std::unexpected(std::format(">> try_parse_num::Error: Could not find key \"{}\"", m_buffer));
            }
            const auto& value = m_map.at(m_buffer);
            auto result       = parse_num_handled<T>(value);
            if(!result)
            {
                return std::unexpected(std::format(">> try_parse_num::Error: Could not parse \"{}\"", result.error()));
            }

            return result.value();
        }

        template<typename T>
        auto try_parse_vector(str_v key, str_v delim) -> std::expected<std::vector<T>, std::string>
            requires std::is_integral_v<T> || std::is_floating_point_v<T>
        {
            m_buffer = key;
            if (!m_map.contains(m_buffer))
            {
                return std::unexpected(std::format(">> try_parse_vec::Error Could not find key \"{}\"", m_buffer));
            }
            const auto& value = m_map.at(m_buffer);
            auto result       = try_parse_vec<T>(value, delim).transform_error([](std::string error)
            {
                auto new_error = std::format(">> try_parse_vec::Error Could not parse following tokens: \"{}\"", error);
                return new_error;
            });
            return result;
        }

        auto try_get_str(str_v key) -> std::expected<std::string, std::string>
        {
            m_buffer = key;
            if (!m_map.contains(m_buffer))
            {
                return std::unexpected(std::format(">> try_parse_str::Error: Could not find key \"{}\"", m_buffer));
            }
            return m_map.at(m_buffer);
        }
    };
}
#endif