#pragma once

#ifndef LOXE_TREE_WALKER_ENVIRONMENT_HPP
#define LOXE_TREE_WALKER_ENVIRONMENT_HPP

#include <string>
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

        auto define(const class Token& name, Object value) -> void;
        auto assign(const class Token& name, Object value) -> Object&;
        auto get   (const class Token& name)               -> Object&;

    private:
        Environment* m_enclosing;
        value_map    m_values;
    };
} // namespace loxe

#endif // !LOXE_TREE_WALKER_ENVIRONMENT_HPP
