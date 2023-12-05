#include "loxe/tree_walker/instance.hpp"
#include "loxe/tree_walker/error.hpp"

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
