#pragma once

#ifndef LOXE_TREE_WALKER_OBJECT_HPP
#define LOXE_TREE_WALKER_OBJECT_HPP

#include <memory>
#include <string>
#include <vector>
#include <variant>

namespace loxe
{
    class Object
    {
    public:
        class Array
        {
        public:
            using container = std::vector<Object>;

        public:
            Array(container values = {})
                : m_values(std::move(values)) {}

            auto length() const                        -> std::size_t;
            auto to_string() const                     -> std::string;
            auto access_at(double index) const         -> const Object&;
            auto assign_at(double index, Object value) -> Object&;

        private:
            container m_values;
        };

        class Module
        {
        public:
            using env_ptr = std::shared_ptr<class Environment>;

        public:
            Module(std::string name, env_ptr environment)
                : m_name(std::move(name)), m_environment(std::move(environment)) {}

            [[nodiscard]] auto to_string() const -> std::string;
            [[nodiscard]] auto env() -> env_ptr&;

        private:
            std::string m_name;
            env_ptr     m_environment;
        };

    public:
        using nil        = std::monostate;
        using boolean    = bool;
        using number     = double;
        using string     = std::string;
        using callable   = std::shared_ptr<class Callable>;
        using instance   = std::shared_ptr<class InstanceObj>;
        using array      = std::shared_ptr<Array>;
        using module_    = std::shared_ptr<Module>;
        using value_type = std::variant<nil, boolean, number, string, callable, instance, array, module_>;

    public:
        Object()               : m_value(nil())            {}
        Object(boolean  value) : m_value(value)            {}
        Object(number   value) : m_value(value)            {}
        Object(string   value) : m_value(std::move(value)) {}
        Object(callable value) : m_value(std::move(value)) {}
        Object(instance value) : m_value(std::move(value)) {}
        Object(array    value) : m_value(std::move(value)) {}
        Object(module_  value) : m_value(std::move(value)) {}

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

        [[nodiscard]] auto is_truthy() const -> bool;
        [[nodiscard]] auto stringify() const -> std::string;

    private:
        value_type m_value;
    };
} // namespace loxe

#endif // !LOXE_TREE_WALKER_OBJECT_HPP
