#pragma once

#ifndef LOXE_TREE_WALKER_OBJECT_HPP
#define LOXE_TREE_WALKER_OBJECT_HPP

#include <string>
#include <variant>

namespace loxe
{
    class Object
    {
    public:
        using nil        = std::monostate;
        using boolean    = bool;
        using number     = double;
        using string     = std::string;
        using value_type = std::variant<nil, boolean, number, string>;

    public:
        Object()              : m_value(nil())            {}
        Object(boolean value) : m_value(value)            {}
        Object(number  value) : m_value(value)            {}
        Object(string  value) : m_value(std::move(value)) {}

        template<typename T>
        [[nodiscard]] constexpr auto is() const -> bool
        {
            return std::holds_alternative<T>(m_value);
        }

        template<typename T>
        [[nodiscard]] constexpr auto as() -> T&
        {
            return std::get<T>(m_value);
        }

        template<typename T>
        [[nodiscard]] constexpr auto as() const -> const T&
        {
            return std::get<T>(m_value);
        }

        [[nodiscard]] constexpr auto operator==(const Object& other) const -> bool = default;
        [[nodiscard]] constexpr auto operator!=(const Object& other) const -> bool = default;

    private:
        value_type m_value;
    };
} // namespace loxe

#endif // !LOXE_TREE_WALKER_OBJECT_HPP