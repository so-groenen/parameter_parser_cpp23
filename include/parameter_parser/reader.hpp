#ifndef PARAMETER_READER_HPP
#define PARAMETER_READER_HPP

#include <iostream>
#include <istream>
#include <ostream>
#include <fstream>
#include <unordered_map>
#include <ranges>
#include <print>
#include <string>
#include <vector>
#include <string_view>
#include <expected>

#include "reader_error.hpp"
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

    enum class Mode
    {
        Permissive,
        Strict,
    };
    auto exit_if_err(ReaderError&& error) -> ReaderError 
    {
        error.from = parameter_parser::map_try_parse_to_parse_or_exit(error.from);
        std::cerr << "Exiting with ParameterParserError: " << error;
        exit(EXIT_FAILURE);
        return error;
    }

    class ParameterReader
    {
        ParameterMap m_map{};
        std::string m_buffer{};
        ParameterReader(ParameterMap&& map)
            : m_map{std::move(map)} // call the unordered_map move constructor
        {   
        }
    private:
        static auto build_from_ifstream(std::ifstream& parameter_file, str_v delimiter, Mode mode) -> std::expected<ParameterReader, ReaderError>
        {
            std::unordered_map<std::string, std::string> parameter_map{};
            std::string buffer{};
            while (std::getline(parameter_file, buffer))
            {
                auto key_val = split_once(buffer, delimiter);
                if (!key_val.has_value() && mode == Mode::Strict)
                {
                    ReaderError error{.args = buffer, .from = ReaderError::From::build, .kind = ReaderError::Kind::ParseError};
                    return std::unexpected(error);
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
        static auto build(const char* file_path, str_v delimiter, Mode mode = Mode::Strict) -> std::expected<ParameterReader, ReaderError>
        {
            std::ifstream parameter_file{file_path};
            if (!parameter_file.is_open())
            {
                ReaderError error{.args = file_path, .from = ReaderError::From::build, .kind = ReaderError::Kind::FileError};
                return std::unexpected(error);
            }

            return build_from_ifstream(parameter_file,delimiter, mode);
        }
        static auto build(const std::string& file_path, str_v delimiter, Mode mode = Mode::Strict) -> std::expected<ParameterReader, ReaderError>
        {
            std::ifstream parameter_file{file_path};
            if (!parameter_file.is_open())
            {
                ReaderError error{.args = file_path, .from = ReaderError::From::build, .kind = ReaderError::Kind::FileError};
                return std::unexpected(error);
            }
            return build_from_ifstream(parameter_file,delimiter, mode);
        }
        static auto build(str_v file_path, str_v delimiter, Mode mode = Mode::Strict) -> std::expected<ParameterReader, ReaderError>
        {
            std::ifstream parameter_file{file_path.data()};
            if (!parameter_file.is_open())
            {
                ReaderError error{.args = std::string{file_path}, .from = ReaderError::From::build, .kind = ReaderError::Kind::FileError};
                return std::unexpected(error);
            }
            return build_from_ifstream(parameter_file,delimiter, mode);
        }

        template<typename T>
        auto try_parse_num(str_v key) -> std::expected<T, ReaderError>
            requires std::is_integral_v<T> || std::is_floating_point_v<T>
        {
            m_buffer = key;
            if (!m_map.contains(m_buffer))
            {
                ReaderError error{.args = m_buffer, .from = ReaderError::From::try_parse_num, .kind = ReaderError::Kind::KeyError};
                return std::unexpected(error);
            }
            const auto& value = m_map.at(m_buffer);
            auto result       = parse_num_handled<T>(value);
            if(!result)
            {
                ReaderError error{.args = std::string{result.error()}, .from = ReaderError::From::try_parse_num, .kind = ReaderError::Kind::ParseError};
                return std::unexpected(error);
            }

            return result.value();
        }
template<typename T>
        auto parse_num_or_exit(str_v key) -> T
            requires std::is_integral_v<T> || std::is_floating_point_v<T>
        {
            return try_parse_num<T>(key).transform_error(exit_if_err).value();

        }
        template<typename T>
        auto try_parse_vector(str_v key, str_v delim) -> std::expected<std::vector<T>, ReaderError>
            requires std::is_integral_v<T> || std::is_floating_point_v<T>
        {
            m_buffer = key;
            if (!m_map.contains(m_buffer))
            {
                ReaderError error{.args = m_buffer, .from = ReaderError::From::try_parse_vec, .kind = ReaderError::Kind::KeyError};
                return std::unexpected(error);
            }
            const auto& value = m_map.at(m_buffer);
            auto result       = try_parse_vec<T>(value, delim).transform_error([](std::string error)
            {
                ReaderError read_error{.args = error, .from = ReaderError::From::try_parse_vec, .kind = ReaderError::Kind::ParseError};
                return read_error;
            });
            return result;
        }
        template<typename T>
        auto parse_vector_or_exit(str_v key, str_v delim) -> std::vector<T>
            requires std::is_integral_v<T> || std::is_floating_point_v<T>
        {
            return try_parse_vector<T>(key, delim).transform_error(exit_if_err).value();
        }
        auto try_get_str(str_v key) -> std::expected<std::string, ReaderError>
        {
            m_buffer = key;
            if (!m_map.contains(m_buffer))
            {
                ReaderError error{.args = m_buffer, .from = ReaderError::From::try_parse_str, .kind = ReaderError::Kind::KeyError};
                return std::unexpected(error);
            }
            return m_map.at(m_buffer);
        }
        auto get_str_or_exit(str_v key) -> std::string
        {
            return try_get_str(key).transform_error(exit_if_err).value();
        }
    };
}
#endif