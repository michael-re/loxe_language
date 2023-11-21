#pragma once

#ifndef LOXE_PARSER_PARSER_HPP
#define LOXE_PARSER_PARSER_HPP

#include "loxe/common/except.hpp"

#include "expr.hpp"
#include "lexer.hpp"
#include "token.hpp"

namespace loxe
{
    class Parser
    {
    public:
        struct ParseError : public Exception
        {
            ParseError(Token token, std::string message);

            const Token token;
        };

    public:
        [[nodiscard]] auto parse(std::string source) -> ast::expr_ptr;

    private:
        [[nodiscard]] auto parse_expression() -> ast::expr_ptr;
        [[nodiscard]] auto parse_equality()   -> ast::expr_ptr;
        [[nodiscard]] auto parse_comparison() -> ast::expr_ptr;
        [[nodiscard]] auto parse_term()       -> ast::expr_ptr;
        [[nodiscard]] auto parse_factor()     -> ast::expr_ptr;
        [[nodiscard]] auto parse_unary()      -> ast::expr_ptr;
        [[nodiscard]] auto parse_primary()    -> ast::expr_ptr;

        [[nodiscard]] auto check(Token::Type type) const -> bool;
        [[nodiscard]] auto match(Token::Type type)       -> bool;

        [[nodiscard]] auto at_end()   const -> bool;
        [[nodiscard]] auto current()  const -> Token;
        [[nodiscard]] auto previous() const -> Token;
        [[nodiscard]] auto next()           -> Token;

    private:
        auto consume(Token::Type type, std::string msg)  -> Token;
        auto error  (Token token, std::string msg) const -> ParseError;

    private:
        Lexer m_lexer;
    };
} // namespace loxe

#endif // !LOXE_PARSER_PARSER_HPP
