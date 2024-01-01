#pragma once

#ifndef LOXE_TREE_WALKER_INTERPRETER_HPP
#define LOXE_TREE_WALKER_INTERPRETER_HPP

#include <memory>

#include "loxe/parser/ast.hpp"

#include "object.hpp"
#include "environment.hpp"

namespace loxe
{
    class Interpreter : public ast::Stmt::const_visitor<void>, public ast::Expr::const_visitor<Object>
    {
    public:
        using env_ptr = std::shared_ptr<Environment>;

    public:
        Interpreter();

        auto interpret(const ast::stmt_list& program)            -> void;
        auto evaluate (const ast::expr_ptr&  expr)               -> Object;
        auto execute  (const ast::stmt_ptr&  stmt)               -> void;
        auto execute  (const ast::stmt_list& stmts, env_ptr env) -> void;

    private:
        auto visit(const ast::BlockStmt&      stmt) -> void override;
        auto visit(const ast::BreakStmt&      stmt) -> void override;
        auto visit(const ast::ClassStmt&      stmt) -> void override;
        auto visit(const ast::ContinueStmt&   stmt) -> void override;
        auto visit(const ast::ExpressionStmt& stmt) -> void override;
        auto visit(const ast::ForStmt&        stmt) -> void override;
        auto visit(const ast::FunctionStmt&   stmt) -> void override;
        auto visit(const ast::IfStmt&         stmt) -> void override;
        auto visit(const ast::ImportStmt&     stmt) -> void override;
        auto visit(const ast::LetStmt&        stmt) -> void override;
        auto visit(const ast::ModuleStmt&     stmt) -> void override;
        auto visit(const ast::PrintStmt&      stmt) -> void override;
        auto visit(const ast::ReturnStmt&     stmt) -> void override;
        auto visit(const ast::VariableStmt&   stmt) -> void override;
        auto visit(const ast::WhileStmt&      stmt) -> void override;

        auto visit(const ast::ArrayExpr&       expr) -> Object override;
        auto visit(const ast::AssignExpr&      expr) -> Object override;
        auto visit(const ast::BinaryExpr&      expr) -> Object override;
        auto visit(const ast::BooleanExpr&     expr) -> Object override;
        auto visit(const ast::CallExpr&        expr) -> Object override;
        auto visit(const ast::CommaExpr&       expr) -> Object override;
        auto visit(const ast::ConditionalExpr& expr) -> Object override;
        auto visit(const ast::FunctionExpr&    expr) -> Object override;
        auto visit(const ast::GetExpr&         expr) -> Object override;
        auto visit(const ast::GroupingExpr&    expr) -> Object override;
        auto visit(const ast::LogicalExpr&     expr) -> Object override;
        auto visit(const ast::NilExpr&         expr) -> Object override;
        auto visit(const ast::NumberExpr&      expr) -> Object override;
        auto visit(const ast::SetExpr&         expr) -> Object override;
        auto visit(const ast::StringExpr&      expr) -> Object override;
        auto visit(const ast::SubscriptExpr&   expr) -> Object override;
        auto visit(const ast::SuperExpr&       expr) -> Object override;
        auto visit(const ast::ThisExpr&        expr) -> Object override;
        auto visit(const ast::UnaryExpr&       expr) -> Object override;
        auto visit(const ast::VariableExpr&    expr) -> Object override;

    private:
        auto look_up_var(const Token& name, const ast::Expr& expr) -> const Object&;

    private:
        env_ptr m_global;
        env_ptr m_environment;
    };
} // namespace loxe

#endif // !LOXE_TREE_WALKER_INTERPRETER_HPP
