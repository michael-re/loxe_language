#pragma once

#ifndef LOXE_TREE_WALKER_ENVIRONMENT_HPP
#define LOXE_TREE_WALKER_ENVIRONMENT_HPP

#include <string>
#include <cstdint>
#include <unordered_map>

#include "loxe/parser/token.hpp"

#include "object.hpp"

namespace loxe::tree_walker
{
    class Environment
    {
    public:
        using value_map = std::unordered_map<std::string, Object>;

    public:
        Environment(Environment* enclosing = nullptr)
            : m_enclosing(enclosing), m_values({}) {}

        auto define(const class Token& name, Object value) -> void;
        auto assign(const class Token& name, Object value) -> const Object&;
        auto access(const class Token& name)  const        -> const Object&;

        auto assign_at(std::size_t depth, const Token& name, Object value) -> const Object&;
        auto access_at(std::size_t depth, const Token& name) const         -> const Object&;

    private:
        auto ancestor(std::size_t distance) const -> Environment*;

    private:
        Environment* m_enclosing;
        value_map    m_values;
    };
} // namespace loxe::tree_walker

#endif // !LOXE_TREE_WALKER_ENVIRONMENT_HPP
