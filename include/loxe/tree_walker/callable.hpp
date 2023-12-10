#pragma once

#ifndef LOXE_TREE_WALKER_CALLABLE_HPP
#define LOXE_TREE_WALKER_CALLABLE_HPP

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <unordered_map>

#include "loxe/parser/ast.hpp"
#include "loxe/parser/token.hpp"

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
        virtual auto call(Interpreter&, args) const -> Object      = 0;
        virtual auto arity()                  const -> std::size_t = 0;
        virtual auto to_string()              const -> std::string = 0;
    };

    class FunctionObj : public Callable
    {
    public:
        using dec_ptr  = std::shared_ptr<ast::FunctionStmt>;
        using env_ptr  = std::shared_ptr<Environment>;
        using inst_ptr = std::shared_ptr<InstanceObj>;

    public:
        FunctionObj(dec_ptr declaration, env_ptr closure, bool init = false)
            : m_init(init), m_closure(std::move(closure)), m_declaration(std::move(declaration)) {}

        auto call(Interpreter&, args) const -> Object      override;
        auto arity()                  const -> std::size_t override;
        auto to_string()              const -> std::string override;

        auto bind(inst_ptr instance) -> Object;

    private:
        bool    m_init;
        env_ptr m_closure;
        dec_ptr m_declaration;
    };

    class ClassObj : public Callable
    {
    public:
        using fun_ptr      = std::shared_ptr<FunctionObj>;
        using super_type   = std::shared_ptr<ClassObj>;
        using methods_type = std::unordered_map<std::string, fun_ptr>;

    public:
        ClassObj(Token name, methods_type methods, super_type superclass)
            : m_name(std::move(name)), m_superclass(std::move(superclass)), m_methods(std::move(methods)) {}

        auto call(Interpreter&, args) const -> Object      override;
        auto arity()                  const -> std::size_t override;
        auto to_string()              const -> std::string override;

        auto name()                         const -> const std::string&;
        auto find_method(const Token& name) const -> fun_ptr;

    private:
        Token        m_name;
        super_type   m_superclass;
        methods_type m_methods;
    };
} // namespace loxe

#endif // !LOXE_TREE_WALKER_CALLABLE_HPP
