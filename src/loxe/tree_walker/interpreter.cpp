#include "loxe/common/utility.hpp"
#include "loxe/tree_walker/interpreter.hpp"

loxe::Interpreter::Interpreter()
    : m_result() {}

auto loxe::Interpreter::interpret(const ast::expr_ptr& expr) -> void
{
    utility::println("{}", evaluate(expr).stringify());
}

auto loxe::Interpreter::evaluate(const ast::expr_ptr& expr) -> Object&
{
    if (expr) expr->accept(*this);
    else      m_result = Object();

    return m_result;
}

auto loxe::Interpreter::visit(const ast::BinaryExpr& expr) -> void
{
    utility::ignore(expr);
    m_result = Object();
}

auto loxe::Interpreter::visit(const ast::BooleanExpr& expr) -> void
{
    m_result = expr.value;
}

auto loxe::Interpreter::visit(const ast::GroupingExpr& expr) -> void
{
    auto result = std::move(evaluate(expr.expression));
    m_result    = std::move(result);
}

auto loxe::Interpreter::visit(const ast::NilExpr& expr) -> void
{
    utility::ignore(expr);
    m_result = Object();
}

auto loxe::Interpreter::visit(const ast::NumberExpr& expr) -> void
{
    m_result = expr.value;
}

auto loxe::Interpreter::visit(const ast::StringExpr& expr) -> void
{
    m_result = expr.value.lexeme;
}

auto loxe::Interpreter::visit(const ast::UnaryExpr& expr) -> void
{
    utility::ignore(expr);
    m_result = Object();
}
