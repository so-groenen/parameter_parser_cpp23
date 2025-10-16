#ifndef READER_ERROR_HPP
#define READER_ERROR_HPP

#include <ostream>
#include <iostream>
#include <string_view>
#include <format>

namespace parameter_parser
{
    struct ReaderError
    {
        enum class Kind
        {
            FileError,
            ParseError,
            KeyError,
        };
        enum class From
        {
            try_parse_num,
            try_parse_vec,
            try_parse_str,
            parse_num_or_exit,
            parse_vec_or_exit,
            parse_str_or_exit,
            build,
        };
        static std::string_view kind_as_str(Kind kind_)
        {
            switch (kind_)
            {
            case Kind::FileError:
                return "FileError";
            case Kind::KeyError:
                return "KeyError";
            case Kind::ParseError:
                return "ParseError";
            default:
                return "";
            }
        }
        static std::string_view from_as_str(From from_)
        {
            switch (from_)
            {
            case From::build:
                return "build";
            case From::try_parse_num:
                return "try_parse_num";
            case From::try_parse_vec:
                return "try_parse_vec";
            case From::try_parse_str:
                return "try_parse_str";
            case From::parse_num_or_exit:
                return "parse_num_or_exit";
            case From::parse_vec_or_exit:
                return "parse_vec_or_exit";
            case From::parse_str_or_exit:
                return "parse_str_or_exit";
            default:
                return "";
            }
        }
        std::string decode() const 
        {
            return std::format("from: \"{}\", kind: \"{}\", args: \"{}\"", from_as_str(from), kind_as_str(kind), args);
        }
        friend std::ostream& operator<< (std::ostream &out, const ReaderError& reader_err)
        {
            out << reader_err.decode();
            return out;
        }
        std::string args{};
        From from{};
        Kind kind{};
    };
    ReaderError::From map_try_parse_to_parse_or_exit(ReaderError::From from)
    {
        switch (from)
        {
            case ReaderError::From::try_parse_num:
                return ReaderError::From::parse_num_or_exit;
            case ReaderError::From::try_parse_vec:
                return ReaderError::From::parse_vec_or_exit;
            case ReaderError::From::try_parse_str:
                return ReaderError::From::parse_str_or_exit;
            default:
                return from;
        }

    }
}


template <>
struct std::formatter<parameter_parser::ReaderError> : std::formatter<std::string>
{
    auto format(const parameter_parser::ReaderError& reader_err, std::format_context& ctx) const
    {
        return std::formatter<std::string>::format(reader_err.decode(), ctx);
    }
};

#endif