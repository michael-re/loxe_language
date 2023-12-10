#include "loxe/tree_walker/instance.hpp"
#include "loxe/tree_walker/error.hpp"

auto loxe::InstanceObj::to_string() const -> std::string
{
    return "<" + m_class.name() + " instance>";
}

auto loxe::InstanceObj::get(const Token& name) -> Object
{
    const auto& id = name.lexeme;
    if (m_fields.contains(id))               return m_fields[id];
    if (auto it = m_class.find_method(name)) return it->bind(shared_from_this());
    throw RuntimeError(name, "undefined property '" + id + "'");
}

auto loxe::InstanceObj::set(const Token& name, Object value) -> Object&
{
    return (m_fields[name.lexeme] = std::move(value));
}
