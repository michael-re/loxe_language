#pragma once

#ifndef LOXE_PARSER_STMT_HPP
#define LOXE_PARSER_STMT_HPP

#include <memory>
#include <vector>

#include "expr.hpp"
#include "token.hpp"

namespace loxe::ast
{
    using stmt_ptr  = std::unique_ptr<struct Stmt>;
    using stmt_list = std::vector<stmt_ptr>;

    struct Stmt
    {
        struct Visitor
        {
            virtual ~Visitor() = default;
            virtual auto visit(const struct BlockStmt&)      -> void = 0;
            virtual auto visit(const struct ExpressionStmt&) -> void = 0;
            virtual auto visit(const struct PrintStmt&)      -> void = 0;
            virtual auto visit(const struct VariableStmt&)   -> void = 0;
        };

        virtual ~Stmt() = default;
        virtual auto accept(Visitor&) const -> void = 0;
    };

    template<typename Derived>
    struct StmtCRTP : public Stmt
    {
        auto accept(Visitor& visitor) const -> void override
        {
            return visitor.visit(*static_cast<const Derived*>(this));
        };

        template<typename... Args>
        [[nodiscard]] static auto make(Args&&... args) -> stmt_ptr
        {
            return std::make_unique<Derived>(std::forward<Args>(args)...);
        }
    };

    struct BlockStmt final : public StmtCRTP<BlockStmt>
    {
        BlockStmt(stmt_list statements)
            : statements(std::move(statements)) {}

        stmt_list statements;
    };

    struct ExpressionStmt final : public StmtCRTP<ExpressionStmt>
    {
        ExpressionStmt(expr_ptr expression)
            : expression(std::move(expression)) {}

        expr_ptr expression;
    };

    struct PrintStmt final : public StmtCRTP<PrintStmt>
    {
        PrintStmt(expr_ptr expression)
            : expression(std::move(expression)) {}

        expr_ptr expression;
    };

    struct VariableStmt final : public StmtCRTP<VariableStmt>
    {
        VariableStmt(Token name, expr_ptr initializer)
            : name(std::move(name)), initializer(std::move(initializer)) {}

        Token    name;
        expr_ptr initializer; 
    };
} // namespace loxe::ast

#endif // !LOXE_PARSER_STMT_HPP
