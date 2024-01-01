#include "loxe/tree_walker/module.hpp"

auto loxe::Module::to_string() const -> std::string
{
    return m_name;
}

auto loxe::Module::env() -> Environment&
{
    return m_environment;
}
