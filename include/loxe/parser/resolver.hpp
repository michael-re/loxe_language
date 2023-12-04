#pragma once

#ifndef LOXE_PARSER_RESOLVER_HPP
#define LOXE_PARSER_RESOLVER_HPP

#include <vector>
#include <unordered_map>

#include "expr.hpp"
#include "stmt.hpp"

namespace loxe
{
    class Resolver : public ast::Stmt::Visitor, public ast::Expr::Visitor
    {
    public:
        using map   = std::unordered_map<std::string, bool>;
        using stack = std::vector<map>;

    public:
        auto resolve_ast(const ast::stmt_list& ast) -> Resolver&;
        auto had_error() const -> bool;

    private:
        auto resolve(const ast::stmt_list& stmts) -> void;
        auto resolve(const ast::stmt_ptr&  stmt)  -> void;
        auto resolve(const ast::expr_ptr&  expr)  -> void;

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

    private:
        enum class FunType
        {
            None,
            Function,
        };

    private:
        auto begin_scope() -> void;
        auto end_scope()   -> void;

        auto declare(const Token& name) -> void;
        auto define(const Token& name)  -> void;

        auto resolve_local(ast::Expr& expr, const Token& name) -> void;
        auto resolve_function(ast::FunctionStmt&, FunType)     -> void;

        auto error(const Token& token, std::string message) -> void;

    private:
        bool    m_error    = false;
        stack   m_scopes   = {};
        FunType m_fun_type = FunType::None;
    };
} // namespace loxe

#endif // !LOXE_PARSER_RESOLVER_HPP
