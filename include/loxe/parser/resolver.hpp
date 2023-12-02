#pragma once

#ifndef LOXE_PARSER_RESOLVER_HPP
#define LOXE_PARSER_RESOLVER_HPP

#include "expr.hpp"
#include "stmt.hpp"

namespace loxe
{
    class Resolver : public ast::Expr::Visitor, public ast::Stmt::Visitor
    {
    public:
        [[nodiscard]] auto resolve(const ast::stmt_list& ast) -> bool;

    private:
        auto visit(ast::BlockStmt&      stmt) -> void override;
        auto visit(ast::ExpressionStmt& stmt) -> void override;
        auto visit(ast::ForStmt&        stmt) -> void override;
        auto visit(ast::FunctionStmt&   stmt) -> void override;
        auto visit(ast::IfStmt&         stmt) -> void override;
        auto visit(ast::PrintStmt&      stmt) -> void override;
        auto visit(ast::ReturnStmt&     stmt) -> void override;
        auto visit(ast::VariableStmt&   stmt) -> void override;
        auto visit(ast::WhileStmt&      stmt) -> void override;

        auto visit(ast::AssignExpr&   expr) -> void override;
        auto visit(ast::BinaryExpr&   expr) -> void override;
        auto visit(ast::BooleanExpr&  expr) -> void override;
        auto visit(ast::CallExpr&     expr) -> void override;
        auto visit(ast::GroupingExpr& expr) -> void override;
        auto visit(ast::LogicalExpr&  expr) -> void override;
        auto visit(ast::NilExpr&      expr) -> void override;
        auto visit(ast::NumberExpr&   expr) -> void override;
        auto visit(ast::StringExpr&   expr) -> void override;
        auto visit(ast::UnaryExpr&    expr) -> void override;
        auto visit(ast::VariableExpr& expr) -> void override;
    };
} // namespace loxe

#endif // !LOXE_PARSER_RESOLVER_HPP
