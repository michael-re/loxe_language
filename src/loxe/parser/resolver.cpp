#include "loxe/common/utility.hpp"
#include "loxe/parser/resolver.hpp"

auto loxe::Resolver::resolve_ast(const ast::stmt_list& ast) -> Resolver&
{
    m_error    = false;
    m_scopes   = {};
    m_fun_type = FunType::None;

    begin_scope();
    resolve(ast);
    end_scope();

    return *this;
}

auto loxe::Resolver::had_error() const -> bool
{
    return m_error;
}

auto loxe::Resolver::resolve(const ast::stmt_list& stmts) -> void
{
    for (const auto& stmt : stmts)
        resolve(stmt);
}

auto loxe::Resolver::resolve(const ast::stmt_ptr& stmt) -> void
{
    if (stmt) stmt->accept(*this);
}

auto loxe::Resolver::resolve(const ast::expr_ptr& expr) -> void
{
    if (expr) expr->accept(*this);
}

auto loxe::Resolver::visit(ast::BlockStmt& stmt) -> void
{
    begin_scope();
    resolve(stmt.statements);
    end_scope();
}

auto loxe::Resolver::visit(ast::ExpressionStmt& stmt) -> void
{
    resolve(stmt.expression);
}

auto loxe::Resolver::visit(ast::ForStmt& stmt) -> void
{
    resolve(stmt.initializer);
    resolve(stmt.condition);
    resolve(stmt.update);
    resolve(stmt.body);
}

auto loxe::Resolver::visit(ast::FunctionStmt& stmt) -> void
{
    declare(stmt.name);
    define(stmt.name);
    resolve_function(stmt, FunType::Function);
}

auto loxe::Resolver::visit(ast::IfStmt& stmt) -> void
{
    resolve(stmt.condition);
    resolve(stmt.then_branch);
    resolve(stmt.else_branch);
}

auto loxe::Resolver::visit(ast::PrintStmt& stmt) -> void
{
    resolve(stmt.expression);
}

auto loxe::Resolver::visit(ast::ReturnStmt& stmt) -> void
{
    if (m_fun_type == FunType::None) error(stmt.keyword, "can't return from top-level code");
    if (stmt.value) resolve(stmt.value);
}

auto loxe::Resolver::visit(ast::VariableStmt& stmt) -> void
{
    declare(stmt.name);
    if (stmt.initializer) resolve(stmt.initializer);
    define(stmt.name);
}

auto loxe::Resolver::visit(ast::WhileStmt& stmt) -> void
{
    resolve(stmt.condition);
    resolve(stmt.body);
}


auto loxe::Resolver::visit(ast::AssignExpr& expr) -> void
{
    resolve(expr.value);
    resolve_local(expr, expr.name);
}

auto loxe::Resolver::visit(ast::BinaryExpr& expr) -> void
{
    resolve(expr.lhs);
    resolve(expr.rhs);
}

auto loxe::Resolver::visit(ast::BooleanExpr& expr) -> void
{
    utility::ignore(expr);
}

auto loxe::Resolver::visit(ast::CallExpr& expr) -> void
{
    resolve(expr.callee);
    for (const auto& arg : expr.args)
        resolve(arg);
}

auto loxe::Resolver::visit(ast::GroupingExpr& expr) -> void
{
    resolve(expr.expression);
}

auto loxe::Resolver::visit(ast::LogicalExpr& expr) -> void
{
    resolve(expr.lhs);
    resolve(expr.rhs);
}

auto loxe::Resolver::visit(ast::NilExpr& expr) -> void
{
    utility::ignore(expr);
}

auto loxe::Resolver::visit(ast::NumberExpr& expr) -> void
{
    utility::ignore(expr);
}

auto loxe::Resolver::visit(ast::StringExpr& expr) -> void
{
    utility::ignore(expr);
}

auto loxe::Resolver::visit(ast::UnaryExpr& expr) -> void
{
    resolve(expr.operand);
}

auto loxe::Resolver::visit(ast::VariableExpr& expr) -> void
{
    if (!m_scopes.empty() &&
         m_scopes.back().contains(expr.name.lexeme) &&
         m_scopes.back()[expr.name.lexeme] == false)
    {
        error(expr.name, "can't read local variable in its own initializer");
    }

    resolve_local(expr, expr.name);
}

auto loxe::Resolver::begin_scope() -> void
{
    m_scopes.push_back({});
}

auto loxe::Resolver::end_scope() -> void
{
    m_scopes.pop_back();
}

auto loxe::Resolver::declare(const Token &name) -> void
{
    if (m_scopes.empty()) return;
    if (m_scopes.back().contains(name.lexeme))
        error(name, "symbol already defined with this name in this scope");

    m_scopes.back()[name.lexeme] = false;
}

auto loxe::Resolver::define(const Token &name) -> void
{
    if (m_scopes.empty()) return;
    m_scopes.back()[name.lexeme] = true;
}

auto loxe::Resolver::resolve_local(ast::Expr& expr, const Token &name) -> void
{
    for (auto i = std::size_t{0}; i < m_scopes.size(); i++)
    {
        if (m_scopes[m_scopes.size() - 1 - i].contains(name.lexeme))
        {
            expr.depth = i;
            break;
        }
    }
}

auto loxe::Resolver::resolve_function(ast::FunctionStmt& func, FunType type) -> void
{
    const auto enclosing = m_fun_type;
    m_fun_type = type;

    begin_scope();
    for (const auto& param : func.params)
    {
        declare(param);
        define(param);
    }

    resolve(func.body);
    end_scope();

    m_fun_type = enclosing;
}

auto loxe::Resolver::error(const Token& token, std::string message) -> void
{
    static constexpr auto format = "[{}, {}] error resolving token '{}': {}.";
    utility::println(std::cerr, format, token.line, token.column, token.lexeme, std::move(message));
    m_error = true;
}
