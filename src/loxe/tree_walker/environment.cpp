#include "loxe/tree_walker/environment.hpp"
#include "loxe/tree_walker/error.hpp"

auto loxe::tree_walker::Environment::define(const Token& name, Object value) -> void
{
    const auto& id = name.lexeme;
    if (m_values.contains(id))
        throw RuntimeError(name, "symbol already define with name '" + id + "' in the current scope");
    m_values[id] = std::move(value);
}

auto loxe::tree_walker::Environment::assign(const Token& name, Object value) -> const Object&
{
    const auto& id = name.lexeme;
    if (m_values.contains(id)) return (m_values[id] = std::move(value));
    if (m_enclosing)           return m_enclosing->assign(name, std::move(value));
    throw RuntimeError(name, "'" + id + "' is an undefined symbol");
}

auto loxe::tree_walker::Environment::access(const Token& name) const -> const Object&
{
    const auto& id = name.lexeme;
    const auto& it = m_values.find(id);
    if (it != m_values.end()) return it->second;
    if (m_enclosing)          return m_enclosing->access(name);
    throw RuntimeError(name, "'" + id + "' is an undefined symbol");
}

auto loxe::tree_walker::Environment::ancestor(std::size_t distance) const -> Environment*
{
    auto ptr = this;
    for (auto i = std::size_t{1}; ptr && i < distance; i++)
        ptr = ptr->m_enclosing;

    return const_cast<Environment*>(ptr);
}

auto loxe::tree_walker::Environment::assign_at(std::size_t depth, const Token& name, Object value) -> const Object&
{
    if (auto ptr = ancestor(depth)) return ptr->assign(name, std::move(value));
    static constexpr auto message = "can't assign '{}' symbol because the enclosing does not exist at depth {}";
    throw RuntimeError(name, utility::as_string(message, name.lexeme, depth));
}

auto loxe::tree_walker::Environment::access_at(std::size_t depth, const Token& name) const -> const Object&
{
    if (auto ptr = ancestor(depth)) return ptr->access(name);
    static constexpr auto message = "can't access '{}' symbol because enclosing does not exist at depth {}";
    throw RuntimeError(name, utility::as_string(message, name.lexeme, depth));
}
