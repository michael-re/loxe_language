#include "loxe/common/utility.hpp"
#include "loxe/parser/resolver.hpp"

auto loxe::Resolver::resolve_ast(const ast::stmt_list& ast) -> Resolver&
{
    m_loops    = 0;
    m_error    = false;
    m_scopes   = {};
    m_fun_type = FunType::None;
    m_cls_type = ClsType::None;

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

auto loxe::Resolver::visit(ast::BreakStmt& stmt) -> void
{
    if (!m_loops)
        error(stmt.keyword, "must be inside loop to use 'break'");
}

auto loxe::Resolver::visit(ast::ClassStmt& stmt) -> void
{
    const auto enclosing = m_cls_type;
    m_cls_type = ClsType::Class;

    declare(stmt.name);
    define(stmt.name);

    auto superclass = dynamic_cast<ast::VariableExpr*>(stmt.superclass.get());
    if (superclass && superclass->name.lexeme == stmt.name.lexeme)
        error(superclass->name, "class can't inherit from itself");

    if (superclass)
    {
        begin_scope();
        m_scopes.back()["super"] = true;
        m_cls_type = ClsType::SubClass;
        resolve(stmt.superclass);
    }

    begin_scope();
    m_scopes.back()["this"] = true;

    for (const auto& method : stmt.methods)
    {
        const auto fun_type = method->name.lexeme == "init" ? FunType::Initializer : FunType::Method;
        resolve_function(*(method.get()), fun_type);
    }

    end_scope();
    if (superclass) end_scope();
    m_cls_type = enclosing;
}

auto loxe::Resolver::visit(ast::ContinueStmt& stmt) -> void
{
    if (!m_loops)
        error(stmt.keyword, "must be inside loop to use 'continue'");
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

    begin_loop();
    resolve(stmt.body);
    end_loop();
}

auto loxe::Resolver::visit(ast::FunctionStmt& stmt) -> void
{
    declare(stmt.function->name);
    define(stmt.function->name);
    resolve_function(*stmt.function.get(), FunType::Function);
}

auto loxe::Resolver::visit(ast::IfStmt& stmt) -> void
{
    resolve(stmt.condition);
    resolve(stmt.then_branch);
    resolve(stmt.else_branch);
}

auto loxe::Resolver::visit(ast::LetStmt& stmt) -> void
{
    declare(stmt.name);

    if (stmt.initializer)
        resolve(stmt.initializer);
    else
        error(stmt.name, "let statement must be initalized to a value");

    define(stmt.name);
}

auto loxe::Resolver::visit(ast::PrintStmt& stmt) -> void
{
    resolve(stmt.expression);
}

auto loxe::Resolver::visit(ast::ReturnStmt& stmt) -> void
{
    if (m_fun_type == FunType::None)
        error(stmt.keyword, "can't return from top-level code");

    if (stmt.value && m_fun_type == FunType::Initializer)
        error(stmt.keyword, "can't return value from class init method");

    if (stmt.value) resolve(stmt.value);
}

auto loxe::Resolver::visit(ast::VariableStmt& stmt) -> void
{
    declare(stmt.name, true);
    if (stmt.initializer) resolve(stmt.initializer);
    define(stmt.name);
}

auto loxe::Resolver::visit(ast::WhileStmt& stmt) -> void
{
    resolve(stmt.condition);
    begin_loop();
    resolve(stmt.body);
    end_loop();
}

auto loxe::Resolver::visit(ast::ArrayExpr& expr) -> void
{
    resolve(expr.size);
    for (const auto& value : expr.initializer)
        resolve(value);
}

auto loxe::Resolver::visit(ast::AssignExpr& expr) -> void
{
    const auto assign = m_assign;

    resolve(expr.value);
    m_assign = true;
    resolve_local(expr, expr.name);
    m_assign = assign;
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

auto loxe::Resolver::visit(ast::CommaExpr& expr) -> void
{
    for (const auto& expression : expr.expressions)
        resolve(expression);
}

auto loxe::Resolver::visit(ast::ConditionalExpr& expr) -> void
{
    resolve(expr.condition);
    resolve(expr.then_branch);
    resolve(expr.else_branch);
}

auto loxe::Resolver::visit(ast::FunctionExpr& expr) -> void
{
    resolve_function(expr, FunType::Function);
}

auto loxe::Resolver::visit(ast::GetExpr& expr) -> void
{
    resolve(expr.object);
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

auto loxe::Resolver::visit(ast::SetExpr& expr) -> void
{
    resolve(expr.value);
    resolve(expr.object);
}

auto loxe::Resolver::visit(ast::StringExpr& expr) -> void
{
    utility::ignore(expr);
}

auto loxe::Resolver::visit(ast::SubscriptExpr& expr) -> void
{
    resolve(expr.expression);
    resolve(expr.index);
    if (expr.new_value) resolve(*expr.new_value);
}

auto loxe::Resolver::visit(ast::SuperExpr& expr) -> void
{
    if (m_cls_type == ClsType::None)
        error(expr.keyword, "can't use super outside of a class");
    else if (m_cls_type != ClsType::SubClass)
        error(expr.keyword, "can't use 'super' in a class with no superclass");

    resolve_local(expr, expr.keyword);
}

auto loxe::Resolver::visit(ast::ThisExpr& expr) -> void
{
    if (m_cls_type == ClsType::None)
    {
        error(expr.keyword, "can't use 'this' outside of class");
        return;
    }
    resolve_local(expr, expr.keyword);
}

auto loxe::Resolver::visit(ast::UnaryExpr& expr) -> void
{
    resolve(expr.operand);
}

auto loxe::Resolver::visit(ast::VariableExpr& expr) -> void
{
    if (!m_scopes.empty() &&
         m_scopes.back().contains(expr.name.lexeme) &&
         m_scopes.back()[expr.name.lexeme].defined == false)
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

auto loxe::Resolver::begin_loop() -> void
{
    m_loops++;
}

auto loxe::Resolver::end_loop() -> void
{
    m_loops--;
}

auto loxe::Resolver::declare(const Token &name, bool assignable) -> void
{
    if (m_scopes.empty()) return;
    if (m_scopes.back().contains(name.lexeme))
        error(name, "symbol already defined with this name in this scope");

    m_scopes.back()[name.lexeme] = { false, assignable };
}

auto loxe::Resolver::define(const Token &name) -> void
{
    if (m_scopes.empty()) return;
    m_scopes.back()[name.lexeme].defined = true;
}

auto loxe::Resolver::resolve_local(ast::Expr& expr, const Token &name) -> void
{
    for (auto i = std::size_t{0}; i < m_scopes.size(); i++)
    {
        if (m_scopes[m_scopes.size() - 1 - i].contains(name.lexeme))
        {
            if (m_assign && !m_scopes[m_scopes.size() - 1 - i][name.lexeme].assignable)
                error(name, "can't reassign this symbol in the current scope");
            expr.depth = i;
            break;
        }
    }
}

auto loxe::Resolver::resolve_function(ast::FunctionExpr& func, FunType type) -> void
{
    const auto enclosing = m_fun_type;
    m_fun_type = type;

    begin_scope();
    for (const auto& param : func.params)
    {
        declare(param, true);
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
