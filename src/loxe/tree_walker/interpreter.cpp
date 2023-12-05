#include "loxe/common/utility.hpp"
#include "loxe/tree_walker/error.hpp"
#include "loxe/tree_walker/callable.hpp"
#include "loxe/tree_walker/interpreter.hpp"

loxe::Interpreter::Interpreter()
    : m_result(), m_global(std::make_shared<Environment>()), m_environment(m_global) {}

auto loxe::Interpreter::interpret(const ast::stmt_list& program) -> void
{
    try
    {
        for (const auto& stmt : program)
            execute(stmt);
    }
    catch (const Exception& e)
    {
        utility::println(std::cerr, "{}", e.what());
    }
}

auto loxe::Interpreter::evaluate(const ast::expr_ptr& expr) -> Object&
{
    if (expr) expr->accept(*this);
    else      m_result = Object();

    return m_result;
}

auto loxe::Interpreter::execute(const ast::stmt_ptr& stmt) -> void
{
    if (stmt) stmt->accept(*this);
}

auto loxe::Interpreter::execute(const ast::stmt_list& stmts, env_ptr env) -> void
{
    auto previous = std::move(m_environment);
    try
    {
        m_environment = std::move(env);
        for (const auto& stmt : stmts)
            execute(stmt);

        m_environment = std::move(previous);
    }
    catch (ReturnError& e)
    {
        m_environment = std::move(previous);
        throw e;
    }
    catch (const RuntimeError& e)
    {
        m_environment = std::move(previous);
        throw e;
    }
}

auto loxe::Interpreter::visit(const ast::BlockStmt& stmt) -> void
{
    execute(stmt.statements, std::make_shared<Environment>(m_environment.get()));
}

auto loxe::Interpreter::visit(const ast::ClassStmt& stmt) -> void
{
    m_environment->define(stmt.name, { std::make_shared<ClassObj>(stmt.name) });
}

auto loxe::Interpreter::visit(const ast::ExpressionStmt& stmt) -> void
{
    evaluate(stmt.expression);
}

auto loxe::Interpreter::visit(const ast::ForStmt& stmt) -> void
{
    for (execute(stmt.initializer); evaluate(stmt.condition).is_truthy(); evaluate(stmt.update))
        execute(stmt.body);
}

auto loxe::Interpreter::visit(const ast::FunctionStmt& stmt) -> void
{
    auto function = std::make_shared<FunctionObj>(stmt.make_clone(), *(m_environment.get()));
    m_environment->define(stmt.name, { function });
}

auto loxe::Interpreter::visit(const ast::IfStmt& stmt) -> void
{
    if (evaluate(stmt.condition).is_truthy())
        execute(stmt.then_branch);
    else if (stmt.else_branch)
        execute(stmt.then_branch);
}

auto loxe::Interpreter::visit(const ast::PrintStmt& stmt) -> void
{
    utility::println("{}", evaluate(stmt.expression).stringify());
}

auto loxe::Interpreter::visit(const ast::ReturnStmt& stmt) -> void
{
    throw ReturnError(stmt.value ? std::move(evaluate(stmt.value)) : Object());
}

auto loxe::Interpreter::visit(const ast::VariableStmt& stmt) -> void
{
    auto value = stmt.initializer ? evaluate(stmt.initializer) : Object();
    m_environment->define(stmt.name, std::move(value));
}

auto loxe::Interpreter::visit(const ast::WhileStmt& stmt) -> void
{
    while (evaluate(stmt.condition).is_truthy())
        execute(stmt.body);
}

auto loxe::Interpreter::visit(const ast::AssignExpr& expr) -> void
{
    m_result = (m_environment->assign_at(*expr.depth, expr.name, evaluate(expr.value)));
}

auto loxe::Interpreter::visit(const ast::BinaryExpr& expr) -> void
{
    auto number = [&expr](const Object& object) -> Object::number {
        if (!object.is<Object::number>()) throw RuntimeError(expr.op, "operator requires a number");
        return object.as<Object::number>();
    };

    auto lhs = std::move(evaluate(expr.lhs));
    auto rhs = std::move(evaluate(expr.rhs));
    m_result = Object();

    switch (expr.op.type)
    {
        case Token::Type::BangEqual:  m_result = (lhs != rhs); break;
        case Token::Type::EqualEqual: m_result = (lhs == rhs); break;
        case Token::Type::Plus:
            if (lhs.is<Object::string>() && rhs.is<Object::string>())
            {
                m_result = (lhs.as<Object::string>() + rhs.as<Object::string>());
                break;
            }

            if (lhs.is<Object::number>() && rhs.is<Object::number>())
            {
                m_result = (lhs.as<Object::number>() + rhs.as<Object::number>());
                break;
            }

            throw RuntimeError(expr.op, "'+' operator requires two numbers or strings");
            break;
        case Token::Type::Minus:        m_result = number(lhs) -  number(rhs); break;
        case Token::Type::Star:         m_result = number(lhs) *  number(rhs); break;
        case Token::Type::Slash:        m_result = number(lhs) /  number(rhs); break;
        case Token::Type::Greater:      m_result = number(lhs) >  number(rhs); break;
        case Token::Type::GreaterEqual: m_result = number(lhs) >= number(rhs); break;
        case Token::Type::Less:         m_result = number(lhs) <  number(rhs); break;
        case Token::Type::LessEqual:    m_result = number(lhs) <= number(rhs); break;
        default: throw RuntimeError(expr.op, "unrecognized binary operator: '" + expr.op.lexeme + "'");
    }
}

auto loxe::Interpreter::visit(const ast::BooleanExpr& expr) -> void
{
    m_result = expr.value;
}

auto loxe::Interpreter::visit(const ast::CallExpr& expr) -> void
{
    auto callee = evaluate(expr.callee);
    auto args   = Callable::args();
    for (const auto& arg : expr.args)
        args.emplace_back(std::move(evaluate(arg)));

    if (!callee.is<Object::callable>())
        throw RuntimeError(expr.paren, "can only call functions and classes");

    auto callable = callee.as<Object::callable>();
    if (callable->arity() != args.size())
        throw RuntimeError(expr.paren, utility::as_string("expected {} args but got {}", callable-> arity(), args.size()));

    m_result = std::move(callable->call(*this, args));
}

auto loxe::Interpreter::visit(const ast::GroupingExpr& expr) -> void
{
    evaluate(expr.expression);
}

auto loxe::Interpreter::visit(const ast::LogicalExpr& expr) -> void
{
    auto &lhs = evaluate(expr.lhs);
    if (expr.op.type == Token::Type::Or  && lhs.is_truthy())  return;
    if (expr.op.type == Token::Type::And && !lhs.is_truthy()) return;
    evaluate(expr.rhs);
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
    m_result = expr.value;
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

auto loxe::Interpreter::visit(const ast::VariableExpr& expr) -> void
{
    m_result = m_environment->get(expr.name);
}

auto loxe::Interpreter::look_up_var(const Token& name, const ast::Expr& expr) -> Object&
{
    m_result = Object();
    return (m_result = expr.depth ? m_environment->get_at(*expr.depth, name) : m_global->get(name));
}
