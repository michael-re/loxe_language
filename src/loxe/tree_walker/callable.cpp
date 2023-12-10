#include "loxe/tree_walker/error.hpp"
#include "loxe/tree_walker/callable.hpp"
#include "loxe/tree_walker/instance.hpp"
#include "loxe/tree_walker/environment.hpp"
#include "loxe/tree_walker/interpreter.hpp"

auto loxe::FunctionObj::call(Interpreter& interpreter, args args) const -> Object
{
    static const auto implicit_this = Token(Token::Type::Implicit, -1, -1, "this");

    if (!m_declaration)
        throw Exception("can't call undefined function");

    auto environment = std::make_shared<Environment>(m_closure.get());
    for (auto i = args::size_type{0}; i < m_declaration->params.size(); i++)
        environment->define(m_declaration->params[i], std::move(args[i]));

    try
    {
        auto body = dynamic_cast<ast::BlockStmt*>(m_declaration->body.get());
        interpreter.execute(body->statements, environment);
    }
    catch (ReturnError& e)
    {
        return m_init ? m_closure->access_at(0, implicit_this) : std::move(e.value);
    }

    return m_init ? m_closure->access_at(0, implicit_this) : Object();
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

auto loxe::FunctionObj::bind(inst_ptr instance) -> Object
{
    static const auto implicit_this = Token(Token::Type::Implicit, -1, -1, "this");
    auto environment = std::make_shared<Environment>(m_closure.get());
    environment->define(implicit_this, { std::move(instance) });
    return { std::make_shared<FunctionObj>(m_declaration, std::move(environment), m_init) };
}

auto loxe::ClassObj::call(Interpreter& interpreter, args args) const -> Object
{
    auto instance = std::make_shared<InstanceObj>(*this);
    if (auto init = m_methods.find("init"); init != m_methods.end())
        init->second->bind(instance).as<Object::callable>()->call(interpreter, std::move(args));

    return { instance };
}

auto loxe::ClassObj::arity() const -> std::size_t
{
    if (auto init = m_methods.find("init"); init != m_methods.end())
        return init->second->arity();
    return 0;
}

auto loxe::ClassObj::to_string() const -> std::string
{
    return "<class " + m_name.lexeme + ">";
}

auto loxe::ClassObj::name() const -> const std::string&
{
    return m_name.lexeme;
}

auto loxe::ClassObj::find_method(const Token& name) const -> fun_ptr
{
    const auto& id = name.lexeme;
    if (auto it = m_methods.find(id); it != m_methods.end()) return it->second;
    if (m_superclass)                                        return m_superclass->find_method(name);
    return nullptr;
}
