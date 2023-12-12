#pragma once

#ifndef LOXE_PARSER_TOKEN_HPP
#define LOXE_PARSER_TOKEN_HPP

#include <string>

namespace loxe
{
    class Token
    {
    public:
        enum class Type
        {
            // single-character tokens
            Colon,
            Comma,
            Dot,
            LeftBrace,
            LeftParen,
            Minus,
            Plus,
            Question,
            RightBrace,
            RightParen,
            Semicolon,
            Slash,
            Star,

            // one or two character tokens
            Bang,
            BangEqual,
            Equal,
            EqualEqual,
            Greater,
            GreaterEqual,
            Less,
            LessEqual,

            // literals
            Identifier,
            Number,
            String,

            // keywords
            And,
            Class,
            Else,
            False,
            For,
            Fun,
            If,
            Nil,
            Or,
            Print,
            Return,
            Super,
            This,
            True,
            Var,
            While,

            // information
            EndOfFile,
            Implicit,
            Unknown,
        };

    public:
        Token(Type type = Type::Unknown, int line = -1, int column = -1, std::string lexeme = {})
            : line(line), column(column), type(type), lexeme(std::move(lexeme)) {}

        [[nodiscard]] auto to_string()   const -> std::string;
        [[nodiscard]] auto type_string() const -> std::string;

    public:
        [[nodiscard]] static auto ident_type(std::string_view identifier) -> Type;

    public:
        int         line   = -1;
        int         column = -1;
        Type        type   = Type::Unknown;
        std::string lexeme = {};
    };
} // namespace loxe

#endif // !LOXE_PARSER_TOKEN_HPP
