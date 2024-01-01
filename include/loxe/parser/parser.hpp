#pragma once

#ifndef LOXE_PARSER_PARSER_HPP
#define LOXE_PARSER_PARSER_HPP

#include <optional>
#include <unordered_set>

#include "loxe/common/except.hpp"

#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"

namespace loxe
{
    class Parser
    {
    public:
        struct ParseError : public Exception
        {
            ParseError(Token token, std::string message, std::string filename);

            const Token token;
        };

    public:
        [[nodiscard]] static auto parse(std::string source, std::string filename = {}) -> std::optional<ast::stmt_list>;

    private:
        [[nodiscard]] auto parse_dec_or_stmt() -> ast::stmt_ptr;
        [[nodiscard]] auto parse_declaration() -> ast::stmt_ptr;
        [[nodiscard]] auto parse_class_dec()   -> ast::stmt_ptr;
        [[nodiscard]] auto parse_fun_dec()     -> ast::stmt_ptr;
        [[nodiscard]] auto parse_import_dec()  -> ast::stmt_ptr;
        [[nodiscard]] auto parse_let_dec()     -> ast::stmt_ptr;
        [[nodiscard]] auto parse_module_dec()  -> ast::stmt_ptr;
        [[nodiscard]] auto parse_var_dec()     -> ast::stmt_ptr;

        [[nodiscard]] auto parse_statement()     -> ast::stmt_ptr;
        [[nodiscard]] auto parse_block_stmt()    -> ast::stmt_ptr;
        [[nodiscard]] auto parse_break_stmt()    -> ast::stmt_ptr;
        [[nodiscard]] auto parse_continue_stmt() -> ast::stmt_ptr;
        [[nodiscard]] auto parse_expr_stmt()     -> ast::stmt_ptr;
        [[nodiscard]] auto parse_for_stmt()      -> ast::stmt_ptr;
        [[nodiscard]] auto parse_if_stmt()       -> ast::stmt_ptr;
        [[nodiscard]] auto parse_print_stmt()    -> ast::stmt_ptr;
        [[nodiscard]] auto parse_return_stmt()   -> ast::stmt_ptr;
        [[nodiscard]] auto parse_while_stmt()    -> ast::stmt_ptr;

        [[nodiscard]] auto parse_expression()  -> ast::expr_ptr;
        [[nodiscard]] auto parse_comma()       -> ast::expr_ptr;
        [[nodiscard]] auto parse_conditional() -> ast::expr_ptr;
        [[nodiscard]] auto parse_assignment()  -> ast::expr_ptr;
        [[nodiscard]] auto parse_logical_or()  -> ast::expr_ptr;
        [[nodiscard]] auto parse_logical_and() -> ast::expr_ptr;
        [[nodiscard]] auto parse_equality()    -> ast::expr_ptr;
        [[nodiscard]] auto parse_comparison()  -> ast::expr_ptr;
        [[nodiscard]] auto parse_term()        -> ast::expr_ptr;
        [[nodiscard]] auto parse_factor()      -> ast::expr_ptr;
        [[nodiscard]] auto parse_unary()       -> ast::expr_ptr;
        [[nodiscard]] auto parse_call()        -> ast::expr_ptr;
        [[nodiscard]] auto parse_primary()     -> ast::expr_ptr;

        [[nodiscard]] auto parse_lambda()      -> ast::expr_ptr;
        [[nodiscard]] auto parse_super()       -> ast::expr_ptr;
        [[nodiscard]] auto parse_array()       -> ast::expr_ptr;
        [[nodiscard]] auto parse_grouping()    -> ast::expr_ptr;
        [[nodiscard]] auto parse_initializer() -> ast::expr_list;

        [[nodiscard]] auto finish_call(ast::expr_ptr callee) -> ast::expr_ptr;
        [[nodiscard]] auto function(const std::string& kind) -> ast::fun_ptr;

        [[nodiscard]] auto check(Token::Type type) const -> bool;
        [[nodiscard]] auto match(Token::Type type)       -> bool;

        [[nodiscard]] auto at_end()   const -> bool;
        [[nodiscard]] auto current()  const -> Token;
        [[nodiscard]] auto previous() const -> Token;
        [[nodiscard]] auto next()           -> Token;

    private:
        auto consume(Token::Type type, std::string msg) -> Token;
        auto error(Token token, std::string msg)        -> ParseError;
        auto synchronize()                              -> void;

        using files = std::unordered_set<std::string>;

    private:
        bool  m_error          = false;
        Lexer m_lexer          = {};
        files m_import_files   = {};
        std::string m_filename = {}; 
    };
} // namespace loxe

#endif // !LOXE_PARSER_PARSER_HPP
