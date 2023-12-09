#include "loxe/tree_walker/environment.hpp"
#include "loxe/tree_walker/error.hpp"
#include "loxe/parser/token.hpp"

auto loxe::Environment::define(const std::string& name, Object value) -> void
{
    if (m_values.contains(name))
        throw RuntimeError(Token(), "symbol already define with name '" + name + "' in the current scope");
    m_values[name] = std::move(value);
}

auto loxe::Environment::define(const Token& name, Object value) -> void
{
    const auto& id = name.lexeme;
    if (m_values.contains(id))
        throw RuntimeError(name, "symbol already define with name '" + id + "' in the current scope");
    m_values[id] = std::move(value);
}

auto loxe::Environment::assign(const Token& name, Object value) -> Object&
{
    const auto& id = name.lexeme;
    if (m_values.contains(id)) return (m_values[id] = std::move(value));
    if (m_enclosing)           return m_enclosing->assign(name, std::move(value));
    throw RuntimeError(name, "'" + id + "' is an undefined symbol");
}

auto loxe::Environment::get(const Token& name) -> Object&
{
    const auto& id = name.lexeme;
    if (m_values.contains(id)) return m_values[id];
    if (m_enclosing)           return m_enclosing->get(name);
    throw RuntimeError(name, "'" + id + "' is an undefined symbol");
}

auto loxe::Environment::get(const std::string& name) -> Object&
{
    if (m_values.contains(name)) return m_values[name];
    if (m_enclosing)             return m_enclosing->get(name);
    throw RuntimeError({}, "'" + name + "' is an undefined symbol");
}

auto loxe::Environment::ancestor(std::size_t distance) -> Environment*
{
    auto ptr = this;
    for (auto i = std::size_t{1}; ptr && i < distance; i++)
        ptr = ptr->m_enclosing;

    return ptr;
}

auto loxe::Environment::assign_at(std::size_t depth, const Token& name, Object value) -> Object&
{
    if (auto ptr = ancestor(depth)) return ptr->assign(name, std::move(value));
    static constexpr auto message = "can't assign '{}' symbol because the enclosing does not exist at depth {}";
    throw RuntimeError(name, utility::as_string(message, name.lexeme, depth));
}

auto loxe::Environment::get_at(std::size_t depth, const Token& name) -> Object&
{
    if (auto ptr = ancestor(depth)) return ptr->get(name);
    static constexpr auto message = "can't access '{}' symbol because enclosing does not exist at depth {}";
    throw RuntimeError(name, utility::as_string(message, name.lexeme, depth));
}

auto loxe::Environment::get_at(std::size_t depth, const std::string& name) -> Object&
{
    if (auto ptr = ancestor(depth)) return ptr->get(name);
    static constexpr auto message = "can't access '{}' symbol because enclosing does not exist at depth {}";
    throw RuntimeError({}, utility::as_string(message, name, depth));
}
