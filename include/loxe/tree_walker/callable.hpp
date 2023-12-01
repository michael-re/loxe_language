#pragma once

#ifndef LOXE_TREE_WALKER_CALLABLE_HPP
#define LOXE_TREE_WALKER_CALLABLE_HPP

#include <string>
#include <vector>
#include <cstdint>

#include "loxe/parser/stmt.hpp"
#include "object.hpp"

namespace loxe
{
    class Interpreter;

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
        FunctionObj(std::shared_ptr<ast::FunctionStmt> declaration)
            : m_declaration(std::move(declaration)) {}

        auto call(Interpreter&, const args&) const -> Object      override;
        auto arity()                         const -> std::size_t override;
        auto to_string()                     const -> std::string override;

    private:
        std::shared_ptr<ast::FunctionStmt> m_declaration;
    };

    class NativeClock : public Callable
    {
    public:
        auto call(Interpreter&, const args&) const -> Object      override;
        auto arity()                         const -> std::size_t override;
        auto to_string()                     const -> std::string override;
    };
} // namespace loxe


#endif // !LOXE_TREE_WALKER_CALLABLE_HPP
