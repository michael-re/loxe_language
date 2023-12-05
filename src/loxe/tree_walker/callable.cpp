#include "loxe/tree_walker/callable.hpp"
#include "loxe/tree_walker/error.hpp"
#include "loxe/tree_walker/environment.hpp"
#include "loxe/tree_walker/interpreter.hpp"

auto loxe::FunctionObj::call(Interpreter& interpreter, const args& args) const -> Object
{
    if (!m_declaration)
        throw Exception("can't call undefined function");

    auto environment = std::make_shared<Environment>(&m_closure);
    for (auto i = args::size_type{0}; i < m_declaration->params.size(); i++)
        environment->define(m_declaration->params[i].lexeme, args[i]);

    try
    {
        auto body = dynamic_cast<ast::BlockStmt*>(m_declaration->body.get());
        interpreter.execute(body->statements, environment);
    }
    catch (ReturnError& e)
    {
        if (m_init) return m_closure.get_at(0, "this");
        return std::move(e.value);
    }

    return m_init ? m_closure.get_at(0, "this") : Object();
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

auto loxe::FunctionObj::bind(std::shared_ptr<InstanceObj> instance) -> Object
{
    auto environment = Environment(&m_closure);
    environment.define("this", { std::move(instance) });
    return { std::make_shared<FunctionObj>(this->m_declaration->make_clone(), std::move(environment), m_init) };
}

auto loxe::ClassObj::call(Interpreter& interpreter, const args& args) const -> Object
{
    auto instance = std::make_shared<InstanceObj>(*this);
    if (auto init = m_methods.find("init"); init != m_methods.end())
        init->second->bind(instance).as<Object::callable>()->call(interpreter, args);

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

auto loxe::ClassObj::methods() -> methods_type&
{
    return m_methods;
}

auto loxe::InstanceObj::call(Interpreter&, const args&) const -> Object
{
    // TODO: implement me
    return {};
}

auto loxe::InstanceObj::arity() const -> std::size_t
{
    // TODO: implement me
    return 0;
}

auto loxe::InstanceObj::to_string() const -> std::string
{
    return "<" + m_class.name() + " instance>";
}

auto loxe::InstanceObj::get(const Token& name) -> Object
{
    if (m_fields.contains(name.lexeme))          return m_fields[name.lexeme];
    if (m_class.methods().contains(name.lexeme)) return m_class.methods()[name.lexeme]->bind(shared_from_this());
    throw RuntimeError(name, "undefined property '" + name.lexeme + "'");
}

auto loxe::InstanceObj::set(const Token& name, Object value) -> Object&
{
    return (m_fields[name.lexeme] = std::move(value));
}
