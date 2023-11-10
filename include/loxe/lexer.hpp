#pragma once

#ifndef LOXE_LEXER_HPP
#define LOXE_LEXER_HPP

#include <string>
#include <optional>

#include "token.hpp"

namespace loxe
{
    class Lexer
    {
    public:
        Lexer(std::string source = {})
            : m_line(1), m_column(1), m_cursor(0), m_source(std::move(source)) {}

        auto lex() -> Lexer&;

        [[nodiscard]] auto peek_prev() const -> const Token&;
        [[nodiscard]] auto peek_curr() const -> const Token&;
        [[nodiscard]] auto peek_next() const -> const Token&;

    private:
        [[nodiscard]] auto lex_token()       -> Token;
        [[nodiscard]] auto lex_number()      -> Token;
        [[nodiscard]] auto lex_identifier()  -> Token;
        [[nodiscard]] auto lex_punctuation() -> Token;

        [[nodiscard]] auto at_end() const -> bool;
        [[nodiscard]] auto peek0()  const -> std::optional<char>;
        [[nodiscard]] auto peek1()  const -> std::optional<char>;

        [[nodiscard]] auto skip_whitespace() -> std::optional<char>;
        [[nodiscard]] auto skip_comment()    -> std::optional<char>;
        [[nodiscard]] auto advance()         -> std::optional<char>;

    private:
        int         m_line   = 1;
        int         m_column = 1;
        std::size_t m_cursor = 0;
        std::string m_source = {};

        struct
        {
            Token previous = { Token::Type::Unknown   };
            Token current  = { Token::Type::EndOfFile };
            Token next     = { Token::Type::EndOfFile };
        } m_cache;
    };
} // namespace loxe

#endif // !LOXE_LEXER_HPP
