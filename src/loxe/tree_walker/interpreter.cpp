#include "loxe/common/utility.hpp"
#include "loxe/tree_walker/interpreter.hpp"

loxe::Interpreter::Interpreter()
    : m_result() {}

auto loxe::Interpreter::interpret(const ast::expr_ptr& expr) -> void
{
    utility::ignore(expr);
}

auto loxe::Interpreter::visit(const ast::BinaryExpr& expr) -> void
{
    utility::ignore(expr);
    m_result = Object();
}

auto loxe::Interpreter::visit(const ast::BooleanExpr& expr) -> void
{
    utility::ignore(expr);
    m_result = Object();
}

auto loxe::Interpreter::visit(const ast::GroupingExpr& expr) -> void
{
    utility::ignore(expr);
    m_result = Object();
}

auto loxe::Interpreter::visit(const ast::NilExpr& expr) -> void
{
    utility::ignore(expr);
    m_result = Object();
}

auto loxe::Interpreter::visit(const ast::NumberExpr& expr) -> void
{
    utility::ignore(expr);
    m_result = Object();
}

auto loxe::Interpreter::visit(const ast::StringExpr& expr) -> void
{
    utility::ignore(expr);
    m_result = Object();
}

auto loxe::Interpreter::visit(const ast::UnaryExpr& expr) -> void
{
    utility::ignore(expr);
    m_result = Object();
}
