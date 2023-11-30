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
            virtual auto visit(const struct ForStmt&)        -> void = 0;
            virtual auto visit(const struct IfStmt&)         -> void = 0;
            virtual auto visit(const struct PrintStmt&)      -> void = 0;
            virtual auto visit(const struct VariableStmt&)   -> void = 0;
            virtual auto visit(const struct WhileStmt&)      -> void = 0;
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

    struct ForStmt final : public StmtCRTP<ForStmt>
    {
        ForStmt(stmt_ptr initializer, expr_ptr condition, expr_ptr update, stmt_ptr body)
            : initializer(std::move(initializer)), condition(std::move(condition)), update(std::move(update)), body(std::move(body)) {}

        stmt_ptr initializer;
        expr_ptr condition;
        expr_ptr update;
        stmt_ptr body;
    };

    struct IfStmt final : public StmtCRTP<IfStmt>
    {
        IfStmt(expr_ptr condition, stmt_ptr then_branch, stmt_ptr else_branch)
            : condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}

        expr_ptr condition;
        stmt_ptr then_branch;
        stmt_ptr else_branch;
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

    struct WhileStmt final : public StmtCRTP<WhileStmt>
    {
        WhileStmt(expr_ptr condition, stmt_ptr body)
            : condition(std::move(condition)), body(std::move(body)) {}

        expr_ptr condition;
        stmt_ptr body;
    };
} // namespace loxe::ast

#endif // !LOXE_PARSER_STMT_HPP
