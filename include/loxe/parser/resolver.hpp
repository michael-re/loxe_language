#pragma once

#ifndef LOXE_PARSER_RESOLVER_HPP
#define LOXE_PARSER_RESOLVER_HPP

#include <vector>
#include <unordered_map>

#include "ast.hpp"

namespace loxe
{
    class Resolver : public ast::Stmt::visitor<void>, public ast::Expr::visitor<void>
    {
    public:
        struct Attribute
        {
            Attribute(bool defined = false, bool assignable = false)
                : defined(defined), assignable(assignable) {}

            bool defined    = false;
            bool assignable = false;
        };

        using map   = std::unordered_map<std::string, Attribute>;
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
        auto visit(ast::BreakStmt&      stmt) -> void override;
        auto visit(ast::ClassStmt&      stmt) -> void override;
        auto visit(ast::ContinueStmt&   stmt) -> void override;
        auto visit(ast::ExpressionStmt& stmt) -> void override;
        auto visit(ast::ForStmt&        stmt) -> void override;
        auto visit(ast::FunctionStmt&   stmt) -> void override;
        auto visit(ast::IfStmt&         stmt) -> void override;
        auto visit(ast::LetStmt&        stmt) -> void override;
        auto visit(ast::PrintStmt&      stmt) -> void override;
        auto visit(ast::ReturnStmt&     stmt) -> void override;
        auto visit(ast::VariableStmt&   stmt) -> void override;
        auto visit(ast::WhileStmt&      stmt) -> void override;

        auto visit(ast::ArrayExpr&       expr) -> void override;
        auto visit(ast::AssignExpr&      expr) -> void override;
        auto visit(ast::BinaryExpr&      expr) -> void override;
        auto visit(ast::BooleanExpr&     expr) -> void override;
        auto visit(ast::CallExpr&        expr) -> void override;
        auto visit(ast::CommaExpr&       expr) -> void override;
        auto visit(ast::ConditionalExpr& expr) -> void override;
        auto visit(ast::FunctionExpr&    expr) -> void override;
        auto visit(ast::GetExpr&         expr) -> void override;
        auto visit(ast::GroupingExpr&    expr) -> void override;
        auto visit(ast::LogicalExpr&     expr) -> void override;
        auto visit(ast::NilExpr&         expr) -> void override;
        auto visit(ast::NumberExpr&      expr) -> void override;
        auto visit(ast::SetExpr&         expr) -> void override;
        auto visit(ast::StringExpr&      expr) -> void override;
        auto visit(ast::SubscriptExpr&   expr) -> void override;
        auto visit(ast::SuperExpr&       expr) -> void override;
        auto visit(ast::ThisExpr&        expr) -> void override;
        auto visit(ast::UnaryExpr&       expr) -> void override;
        auto visit(ast::VariableExpr&    expr) -> void override;

    private:
        enum class FunType
        {
            Function,
            Initializer,
            Method,
            None,
        };

        enum class ClsType
        {
            Class,
            None,
            SubClass,
        };

    private:
        auto begin_scope() -> void;
        auto end_scope()   -> void;
        auto begin_loop()  -> void;
        auto end_loop()    -> void;

        auto declare(const Token& name, bool assignable = false) -> void;
        auto define(const Token& name)                           -> void;

        auto resolve_local(ast::Expr& expr, const Token& name) -> void;
        auto resolve_function(ast::FunctionExpr&, FunType)     -> void;

        auto error(const Token& token, std::string message) -> void;

    private:
        int     m_loops    = 0;
        bool    m_error    = false;
        bool    m_assign   = false;
        stack   m_scopes   = {};
        FunType m_fun_type = FunType::None;
        ClsType m_cls_type = ClsType::None;
    };
} // namespace loxe

#endif // !LOXE_PARSER_RESOLVER_HPP
