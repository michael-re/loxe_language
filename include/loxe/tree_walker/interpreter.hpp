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
    class Interpreter : public ast::Stmt::ConstVisitor, public ast::Expr::ConstVisitor
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
        auto visit(const ast::ClassStmt&      stmt) -> void override;
        auto visit(const ast::ExpressionStmt& stmt) -> void override;
        auto visit(const ast::ForStmt&        stmt) -> void override;
        auto visit(const ast::FunctionStmt&   stmt) -> void override;
        auto visit(const ast::IfStmt&         stmt) -> void override;
        auto visit(const ast::PrintStmt&      stmt) -> void override;
        auto visit(const ast::ReturnStmt&     stmt) -> void override;
        auto visit(const ast::VariableStmt&   stmt) -> void override;
        auto visit(const ast::WhileStmt&      stmt) -> void override;

        auto visit(const ast::AssignExpr&   expr) -> void override;
        auto visit(const ast::BinaryExpr&   expr) -> void override;
        auto visit(const ast::BooleanExpr&  expr) -> void override;
        auto visit(const ast::CallExpr&     expr) -> void override;
        auto visit(const ast::GetExpr&      expr) -> void override;
        auto visit(const ast::GroupingExpr& expr) -> void override;
        auto visit(const ast::LogicalExpr&  expr) -> void override;
        auto visit(const ast::NilExpr&      expr) -> void override;
        auto visit(const ast::NumberExpr&   expr) -> void override;
        auto visit(const ast::SetExpr&      expr) -> void override;
        auto visit(const ast::StringExpr&   expr) -> void override;
        auto visit(const ast::UnaryExpr&    expr) -> void override;
        auto visit(const ast::VariableExpr& expr) -> void override;

    private:
        auto look_up_var(const Token& name, const ast::Expr& expr) -> Object&;

    private:
        Object  m_result;
        env_ptr m_global;
        env_ptr m_environment;
    };
} // namespace loxe

#endif // !LOXE_TREE_WALKER_INTERPRETER_HPP
