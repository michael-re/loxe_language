#pragma once

#ifndef LOXE_TREE_WALKER_ENVIRONMENT_HPP
#define LOXE_TREE_WALKER_ENVIRONMENT_HPP

#include <string>
#include <cstdint>
#include <unordered_map>

#include "object.hpp"

namespace loxe
{
    class Environment
    {
    public:
        using value_map = std::unordered_map<std::string, Object>;

    public:
        Environment(Environment* enclosing = nullptr)
            : m_enclosing(enclosing), m_values({}) {}

        auto define(const std::string& name, Object value) -> void;
        auto define(const class Token& name, Object value) -> void;
        auto assign(const class Token& name, Object value) -> Object&;
        auto get   (const class Token& name)               -> Object&;
        auto get   (const std::string& name)               -> Object&;

        auto ancestor (std::size_t distance)                               -> Environment*;
        auto assign_at(std::size_t depth, const Token& name, Object value) -> Object&;
        auto get_at   (std::size_t depth, const Token& name)               -> Object&;
        auto get_at   (std::size_t depth, const std::string& name)         -> Object&;

    private:
        Environment* m_enclosing;
        value_map    m_values;
    };
} // namespace loxe

#endif // !LOXE_TREE_WALKER_ENVIRONMENT_HPP
