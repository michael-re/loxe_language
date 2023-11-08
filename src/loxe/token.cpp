#include <array>

#include "loxe/token.hpp"
#include "loxe/utility.hpp"

auto loxe::Token::to_string() const -> std::string
{
    static constexpr auto format = "[{:03d}] [{:5}, {:<3}] {:20} '{}'";
    const auto type_int = static_cast<int>(type);
    return utility::as_string(format, type_int, line, column, type_string(), lexeme);
}

auto loxe::Token::type_string() const -> std::string
{
    static constexpr auto token_max = static_cast<int>(Type::Unknown);
    static constexpr auto token_str = std::array
    {
        // single-character tokens
        "Type::Comma",
        "Type::Dot",
        "Type::LeftBrace",
        "Type::LeftParen",
        "Type::Minus",
        "Type::Plus",
        "Type::RightBrace",
        "Type::RightParen",
        "Type::Semicolon",
        "Type::Slash",
        "Type::Star",

        // one or two character tokens
        "Type::Bang",
        "Type::BangEqual",
        "Type::Equal",
        "Type::EqualEqual",
        "Type::Greater",
        "Type::GreaterEqual",
        "Type::Less",
        "Type::LessEqual",

        // literals
        "Type::Identifier",
        "Type::Number",
        "Type::String",

        // keywords
        "Type::And",
        "Type::Class",
        "Type::Else",
        "Type::False",
        "Type::For",
        "Type::Fun",
        "Type::If",
        "Type::Nil",
        "Type::Or",
        "Type::Print",
        "Type::Return",
        "Type::Super",
        "Type::This",
        "Type::True",
        "Type::Var",
        "Type::While",

        // information
        "Type::EndOfFile",
        "Type::Unknown",
    };

    const auto index = static_cast<decltype(token_str)::size_type>(type);
    return index < token_max ? token_str[index] : token_str.back();
}
