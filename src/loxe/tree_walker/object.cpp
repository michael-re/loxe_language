#include "loxe/tree_walker/object.hpp"
#include "loxe/tree_walker/callable.hpp"
#include "loxe/tree_walker/instance.hpp"
#include "loxe/tree_walker/environment.hpp"

auto loxe::tree_walker::Object::is_truthy() const -> bool
{
    if (is<nil>())     return false;
    if (is<boolean>()) return as<boolean>();
    return true;
}

auto loxe::tree_walker::Object::stringify() const -> std::string
{
    switch (m_value.index())
    {
        case 0: return "nil";
        case 1: return as<boolean>() ? "true" : "false";
        case 2:
        {
            auto string = std::to_string(as<number>());
            while (string.back() == '0') string.pop_back();
            if    (string.back() == '.') string.pop_back();
            return string;
        }
        case 3: return as<string>();
        case 4: return as<callable>()->to_string();
        case 5: return as<instance>()->to_string();
        case 6: return as<array>()->to_string();
        case 7: return as<module_>()->to_string();
        default: break;
    }

    return {};
}

auto loxe::tree_walker::Object::Array::length() const -> std::size_t
{
    return m_values.size();
}

auto loxe::tree_walker::Object::Array::to_string() const -> std::string
{
    auto string = std::string("[");
    for (const auto& value : m_values)
        string += value.stringify() + ", ";

    string.pop_back();
    string.pop_back();
    return string.append("]");
}

auto loxe::tree_walker::Object::Array::access_at(double index) const -> const Object&
{
    return m_values[static_cast<std::size_t>(index)];
}

auto loxe::tree_walker::Object::Array::assign_at(double index, Object value) -> Object&
{
    return (m_values[static_cast<std::size_t>(index)] = std::move(value));
}

auto loxe::tree_walker::Object::Module::to_string() const -> std::string
{
    return "<module " + m_name + ">";
}

auto loxe::tree_walker::Object::Module::env() -> env_ptr&
{
    return m_environment;
}
