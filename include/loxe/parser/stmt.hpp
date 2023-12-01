#pragma once

#ifndef LOXE_PARSER_STMT_HPP
#define LOXE_PARSER_STMT_HPP

#include <memory>
#include <vector>

#include "expr.hpp"
#include "token.hpp"

namespace loxe::ast
{
    using stmt_ptr   = std::unique_ptr<struct Stmt>;
    using stmt_list  = std::vector<stmt_ptr>;
    using param_list = std::vector<Token>;

    struct Stmt
    {
        struct Visitor
        {
            virtual ~Visitor() = default;
            virtual auto visit(const struct BlockStmt&)      -> void = 0;
            virtual auto visit(const struct ExpressionStmt&) -> void = 0;
            virtual auto visit(const struct ForStmt&)        -> void = 0;
            virtual auto visit(const struct FunctionStmt&)   -> void = 0;
            virtual auto visit(const struct IfStmt&)         -> void = 0;
            virtual auto visit(const struct PrintStmt&)      -> void = 0;
            virtual auto visit(const struct ReturnStmt&)     -> void = 0;
            virtual auto visit(const struct VariableStmt&)   -> void = 0;
            virtual auto visit(const struct WhileStmt&)      -> void = 0;
        };

        virtual ~Stmt() = default;
        virtual auto accept(Visitor&) const -> void = 0;

        [[nodiscard]] virtual auto clone() const -> stmt_ptr = 0;
    };

    template<typename Derived>
    struct StmtCRTP : public Stmt
    {
        auto accept(Visitor& visitor) const -> void override
        {
            return visitor.visit(*static_cast<const Derived*>(this));
        }

        [[nodiscard]] auto clone() const -> stmt_ptr override
        {
            return this->make_clone();
        }

        template<typename... Args>
        [[nodiscard]] static auto make(Args&&... args) -> stmt_ptr
        {
            return std::make_unique<Derived>(std::forward<Args>(args)...);
        }

    protected:
        [[nodiscard]] virtual auto make_clone() const -> std::unique_ptr<Derived> = 0;
    };

    struct BlockStmt final : public StmtCRTP<BlockStmt>
    {
        BlockStmt(stmt_list statements)
            : statements(std::move(statements)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<BlockStmt> override
        {
            auto stmts = stmt_list();
            for (const auto& stmt : statements)
                stmts.emplace_back(stmt ? stmt->clone() : nullptr);
            return std::make_unique<BlockStmt>(std::move(stmts));
        }

        stmt_list statements;
    };

    struct ExpressionStmt final : public StmtCRTP<ExpressionStmt>
    {
        ExpressionStmt(expr_ptr expression)
            : expression(std::move(expression)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<ExpressionStmt> override
        {
            auto expr = expression ? expression->clone() : nullptr;
            return std::make_unique<ExpressionStmt>(std::move(expr));
        }

        expr_ptr expression;
    };

    struct ForStmt final : public StmtCRTP<ForStmt>
    {
        ForStmt(stmt_ptr initializer, expr_ptr condition, expr_ptr update, stmt_ptr body)
            : initializer(std::move(initializer)), condition(std::move(condition)), update(std::move(update)), body(std::move(body)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<ForStmt> override
        {
            auto init = initializer ? initializer->clone() : nullptr;
            auto con  = condition   ? condition->clone()   : nullptr;
            auto upd  = update      ? update->clone()      : nullptr;
            auto bdy  = body        ? body->clone()        : nullptr;
            return std::make_unique<ForStmt>(std::move(init), std::move(con), std::move(upd), std::move(bdy));
        }

        stmt_ptr initializer;
        expr_ptr condition;
        expr_ptr update;
        stmt_ptr body;
    };

    struct FunctionStmt final : public StmtCRTP<FunctionStmt>
    {
        FunctionStmt(Token name, param_list params, stmt_ptr body)
            : name(std::move(name)), params(std::move(params)), body(std::move(body)) {}

        auto make_clone() const -> std::unique_ptr<FunctionStmt> override
        {
            auto bdy = body ? body->clone() : nullptr;
            return std::make_unique<FunctionStmt>(name, params, std::move(bdy));
        }

        Token      name;
        param_list params;
        stmt_ptr   body;
    };

    struct IfStmt final : public StmtCRTP<IfStmt>
    {
        IfStmt(expr_ptr condition, stmt_ptr then_branch, stmt_ptr else_branch)
            : condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<IfStmt> override
        {
            auto con = condition   ? condition->clone()   : nullptr;
            auto thn = then_branch ? then_branch->clone() : nullptr;
            auto els = else_branch ? else_branch->clone() : nullptr;
            return std::make_unique<IfStmt>(std::move(con), std::move(thn), std::move(els));
        }

        expr_ptr condition;
        stmt_ptr then_branch;
        stmt_ptr else_branch;
    };

    struct PrintStmt final : public StmtCRTP<PrintStmt>
    {
        PrintStmt(expr_ptr expression)
            : expression(std::move(expression)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<PrintStmt> override
        {
            auto expr = expression ? expression->clone() : nullptr;
            return std::make_unique<PrintStmt>(std::move(expr));
        }

        expr_ptr expression;
    };

    struct ReturnStmt final : public StmtCRTP<ReturnStmt>
    {
        ReturnStmt(Token keyword, expr_ptr value)
            : keyword(std::move(keyword)), value(std::move(value)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<ReturnStmt> override
        {
            auto val = value ? value->clone() : nullptr;
            return std::make_unique<ReturnStmt>(keyword, std::move(val));
        }

        Token    keyword;
        expr_ptr value;
    };

    struct VariableStmt final : public StmtCRTP<VariableStmt>
    {
        VariableStmt(Token name, expr_ptr initializer)
            : name(std::move(name)), initializer(std::move(initializer)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<VariableStmt> override
        {
            auto init = initializer ? initializer->clone() : nullptr;
            return std::make_unique<VariableStmt>(name, std::move(init));
        }

        Token    name;
        expr_ptr initializer;
    };

    struct WhileStmt final : public StmtCRTP<WhileStmt>
    {
        WhileStmt(expr_ptr condition, stmt_ptr body)
            : condition(std::move(condition)), body(std::move(body)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<WhileStmt> override
        {
            auto con = condition ? condition->clone() : nullptr;
            auto bdy = body      ? body->clone()      : nullptr;
            return std::make_unique<WhileStmt>(std::move(con), std::move(bdy));
        }

        expr_ptr condition;
        stmt_ptr body;
    };
} // namespace loxe::ast

#endif // !LOXE_PARSER_STMT_HPP
