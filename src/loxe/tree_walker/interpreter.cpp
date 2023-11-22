#include "loxe/common/utility.hpp"
#include "loxe/tree_walker/error.hpp"
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
    auto operand = std::move(evaluate(expr.operand));
    switch (expr.op.type)
    {
        case Token::Type::Minus:
            if (!operand.is<Object::number>())
                throw RuntimeError(expr.op, "'-' unary operator requires a numbers");
            m_result = -operand.as<Object::number>();
            break;
        case Token::Type::Bang:
            m_result = !operand.is_truthy();
            break;
        default:
            throw RuntimeError(expr.op, "invalid unary operator");
            break;
    }
}
