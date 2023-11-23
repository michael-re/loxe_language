#pragma once

#ifndef LOXE_TREE_WALKER_INTERPRETER_HPP
#define LOXE_TREE_WALKER_INTERPRETER_HPP

#include "loxe/parser/expr.hpp"
#include "loxe/parser/stmt.hpp"

#include "object.hpp"

namespace loxe
{
    class Interpreter : public ast::Expr::Visitor
    {
    public:
        Interpreter();

        auto interpret(const ast::stmt_ptr& stmt) -> void;
        auto evaluate (const ast::expr_ptr& expr) -> Object&;

    private:
        auto visit(const ast::BinaryExpr&   expr) -> void override;
        auto visit(const ast::BooleanExpr&  expr) -> void override;
        auto visit(const ast::GroupingExpr& expr) -> void override;
        auto visit(const ast::NilExpr&      expr) -> void override;
        auto visit(const ast::NumberExpr&   expr) -> void override;
        auto visit(const ast::StringExpr&   expr) -> void override;
        auto visit(const ast::UnaryExpr&    expr) -> void override;

    private:
        Object m_result;
    };
} // namespace loxe

#endif // !LOXE_TREE_WALKER_INTERPRETER_HPP
