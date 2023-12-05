#pragma once

#ifndef LOXE_TREE_WALKER_CALLABLE_HPP
#define LOXE_TREE_WALKER_CALLABLE_HPP

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <unordered_map>

#include "loxe/parser/stmt.hpp"

#include "environment.hpp"
#include "object.hpp"

namespace loxe
{
    class Interpreter;
    class Environment;
    class InstanceObj;

    class Callable
    {
    public:
        using args = std::vector<Object>;

    public:
        virtual ~Callable() = default;
        virtual auto call(Interpreter&, const args&) const -> Object      = 0;
        virtual auto arity()                         const -> std::size_t = 0;
        virtual auto to_string()                     const -> std::string = 0;
    };

    class FunctionObj : public Callable
    {
    public:
        FunctionObj(std::shared_ptr<ast::FunctionStmt> declaration, Environment closure, bool init = false)
            : m_init(init), m_closure(std::move(closure)), m_declaration(std::move(declaration)) {}

        auto call(Interpreter&, const args&) const -> Object      override;
        auto arity()                         const -> std::size_t override;
        auto to_string()                     const -> std::string override;

        auto bind(std::shared_ptr<InstanceObj>) -> Object;

    private:
        bool                               m_init;
        mutable Environment                m_closure;
        std::shared_ptr<ast::FunctionStmt> m_declaration;
    };

    class ClassObj : public Callable
    {
    public:
        using methods_type = std::unordered_map<std::string, std::shared_ptr<FunctionObj>>;

    public:
        ClassObj(Token name, methods_type methods)
            : m_name(std::move(name)), m_methods(std::move(methods)) {}

        auto call(Interpreter&, const args&) const -> Object      override;
        auto arity()                         const -> std::size_t override;
        auto to_string()                     const -> std::string override;

        auto name() const -> const std::string&;
        auto methods() -> methods_type&;

    private:
        Token        m_name;
        methods_type m_methods;
    };

    class InstanceObj : public Callable, public std::enable_shared_from_this<InstanceObj>
    {
    public:
        using class_type = ClassObj;
        using field_type = std::unordered_map<std::string, Object>;

    public:
        InstanceObj(class_type class_obj)
            : m_class(std::move(class_obj)), m_fields({}) {}

        auto call(Interpreter&, const args&) const -> Object      override;
        auto arity()                         const -> std::size_t override;
        auto to_string()                     const -> std::string override;

        auto get(const Token& name)               -> Object;
        auto set(const Token& name, Object value) -> Object&;

    private:
        class_type m_class;
        field_type m_fields;
    };
} // namespace loxe

#endif // !LOXE_TREE_WALKER_CALLABLE_HPP
