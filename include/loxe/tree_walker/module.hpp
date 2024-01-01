#pragma once

#ifndef LOXE_TREE_WALKER_MODULE_HPP
#define LOXE_TREE_WALKER_MODULE_HPP

#include <string>

#include "environment.hpp"

namespace loxe
{
    class Module
    {
    public:
        Module(std::string name, Environment environment)
            : m_name(std::move(name)), m_environment(std::move(environment)) {}

        [[nodiscard]] auto to_string() const -> std::string;
        [[nodiscard]] auto env() -> Environment&;

    private:
        std::string m_name;
        Environment m_environment;
    };
} // namespace loxe

#endif // !LOXE_TREE_WALKER_MODULE_HPP
