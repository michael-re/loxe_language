#pragma once

#ifndef LOXE_TREE_WALKER_CALLABLE_HPP
#define LOXE_TREE_WALKER_CALLABLE_HPP

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

#include "loxe/parser/stmt.hpp"

#include "environment.hpp"
#include "object.hpp"

namespace loxe
{
    class Interpreter;
    class Environment;

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
        FunctionObj(std::shared_ptr<ast::FunctionStmt> declaration, Environment closure)
            : m_closure(std::move(closure)), m_declaration(std::move(declaration)) {}

        auto call(Interpreter&, const args&) const -> Object      override;
        auto arity()                         const -> std::size_t override;
        auto to_string()                     const -> std::string override;

    private:
        mutable Environment                m_closure;
        std::shared_ptr<ast::FunctionStmt> m_declaration;
    };

    class ClassObj : public Callable
    {
    public:
        ClassObj(Token name)
            : name(std::move(name)) {}

        auto call(Interpreter&, const args&) const -> Object      override;
        auto arity()                         const -> std::size_t override;
        auto to_string()                     const -> std::string override;

        friend class InstanceObj;

    private:
        Token name;
    };

    class InstanceObj : public Callable
    {
    public:
        InstanceObj(ClassObj class_obj)
            : class_obj(std::move(class_obj)) {}

        auto call(Interpreter&, const args&) const -> Object      override;
        auto arity()                         const -> std::size_t override;
        auto to_string()                     const -> std::string override;

    private:
        ClassObj class_obj;
    };
} // namespace loxe

#endif // !LOXE_TREE_WALKER_CALLABLE_HPP
