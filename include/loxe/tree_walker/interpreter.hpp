#pragma once

#ifndef LOXE_TREE_WALKER_INTERPRETER_HPP
#define LOXE_TREE_WALKER_INTERPRETER_HPP

#include <memory>

#include "loxe/parser/expr.hpp"
#include "loxe/parser/stmt.hpp"

#include "object.hpp"
#include "environment.hpp"

namespace loxe
{
    class Interpreter : public ast::Expr::Visitor, public ast::Stmt::Visitor
    {
    public:
        using env_ptr = std::shared_ptr<Environment>;

    public:
        Interpreter();

        auto interpret(const ast::stmt_list& program)            -> void;
        auto evaluate (const ast::expr_ptr&  expr)               -> Object&;
        auto execute  (const ast::stmt_ptr&  stmt)               -> void;
        auto execute  (const ast::stmt_list& stmts, env_ptr env) -> void;

    private:
        auto visit(const ast::BlockStmt&      stmt) -> void override;
        auto visit(const ast::ExpressionStmt& stmt) -> void override;
        auto visit(const ast::IfStmt&         stmt) -> void override;
        auto visit(const ast::PrintStmt&      stmt) -> void override;
        auto visit(const ast::VariableStmt&   stmt) -> void override;

        auto visit(const ast::AssignExpr&   expr) -> void override;
        auto visit(const ast::BinaryExpr&   expr) -> void override;
        auto visit(const ast::BooleanExpr&  expr) -> void override;
        auto visit(const ast::GroupingExpr& expr) -> void override;
        auto visit(const ast::LogicalExpr&  expr) -> void override;
        auto visit(const ast::NilExpr&      expr) -> void override;
        auto visit(const ast::NumberExpr&   expr) -> void override;
        auto visit(const ast::StringExpr&   expr) -> void override;
        auto visit(const ast::UnaryExpr&    expr) -> void override;
        auto visit(const ast::VariableExpr& expr) -> void override;

    private:
        Object  m_result;
        env_ptr m_environment;
    };
} // namespace loxe

#endif // !LOXE_TREE_WALKER_INTERPRETER_HPP
