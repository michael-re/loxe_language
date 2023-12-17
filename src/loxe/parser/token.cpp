#include <array>

#include "loxe/common/utility.hpp"
#include "loxe/parser/token.hpp"

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
        "Type::Colon",
        "Type::Comma",
        "Type::Dot",
        "Type::LeftBrace",
        "Type::LeftBracket",
        "Type::LeftParen",
        "Type::Minus",
        "Type::Plus",
        "Type::Question",
        "Type::RightBrace",
        "Type::RightBracket",
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
        "Type::Break",
        "Type::Class",
        "Type::Else",
        "Type::False",
        "Type::For",
        "Type::Fun",
        "Type::If",
        "Type::Lambda",
        "Type::Let",
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
        "Type::Implicit",
        "Type::Unknown",
    };

    const auto index = static_cast<decltype(token_str)::size_type>(type);
    return index < token_max ? token_str[index] : token_str.back();
}

auto loxe::Token::ident_type(std::string_view identifier) -> Type
{
    if (identifier == "and")      return Type::And;
    if (identifier == "break")    return Type::Break;
    if (identifier == "class")    return Type::Class;
    if (identifier == "continue") return Type::Continue;
    if (identifier == "else")     return Type::Else;
    if (identifier == "false")    return Type::False;
    if (identifier == "for")      return Type::For;
    if (identifier == "fun")      return Type::Fun;
    if (identifier == "if")       return Type::If;
    if (identifier == "lambda")   return Type::Lambda;
    if (identifier == "let")      return Type::Let;
    if (identifier == "nil")      return Type::Nil;
    if (identifier == "or")       return Type::Or;
    if (identifier == "print")    return Type::Print;
    if (identifier == "return")   return Type::Return;
    if (identifier == "super")    return Type::Super;
    if (identifier == "this")     return Type::This;
    if (identifier == "true")     return Type::True;
    if (identifier == "var")      return Type::Var;
    if (identifier == "while")    return Type::While;
    return Type::Identifier;
}
