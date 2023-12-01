#include <chrono>

#include "loxe/tree_walker/callable.hpp"
#include "loxe/tree_walker/error.hpp"
#include "loxe/tree_walker/environment.hpp"
#include "loxe/tree_walker/interpreter.hpp"

auto loxe::FunctionObj::call(Interpreter& interpreter, const args& args) const -> Object
{
    if (!m_declaration)
        throw Exception("can't call undefined function");

    auto environment = std::make_shared<Environment>(interpreter.globals().get());
    for (auto i = args::size_type{0}; i < m_declaration->params.size(); i++)
        environment->define(m_declaration->params[i].lexeme, args[i]);

    auto body = dynamic_cast<ast::BlockStmt*>(m_declaration->body.get());
    interpreter.execute(body->statements, environment);
    return {};
}

auto loxe::FunctionObj::arity() const -> std::size_t
{
    if (!m_declaration) return 0;
    return m_declaration->params.size();
}

auto loxe::FunctionObj::to_string() const -> std::string
{
    if (!m_declaration) return "<error: function does not have a declaration>";
    return "<fn " + m_declaration->name.lexeme + ">";
}

auto loxe::NativeClock::call(Interpreter&, const args&) const -> Object
{
    const auto time = std::chrono::high_resolution_clock::now().time_since_epoch();
    const auto ms   = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
    return { static_cast<Object::number>(ms) };
}

auto loxe::NativeClock::arity() const -> std::size_t
{
    return 0;
}

auto loxe::NativeClock::to_string() const -> std::string
{
    return "<fn native clock>";
}
