#include "loxe/tree_walker/environment.hpp"
#include "loxe/tree_walker/error.hpp"
#include "loxe/parser/token.hpp"

auto loxe::Environment::define(const Token& name, Object value) -> void
{
    const auto& id = name.lexeme;
    if (m_values.contains(id))
        throw RuntimeError(name, "symbol already define with name '" + id + "' in the current scope");
    m_values[id] = std::move(value);
}

auto loxe::Environment::get(const Token& name) -> Object&
{
    const auto& id = name.lexeme;
    if (m_values.contains(id)) return m_values[id];
    throw RuntimeError(name, "'" + id + "' is an undefined symbol");
}
