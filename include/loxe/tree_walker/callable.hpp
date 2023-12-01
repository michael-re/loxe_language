#pragma once

#ifndef LOXE_TREE_WALKER_CALLABLE_HPP
#define LOXE_TREE_WALKER_CALLABLE_HPP

#include <string>
#include <vector>
#include <cstdint>

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

    class NativeClock : public Callable
    {
    public:
        auto call(Interpreter&, const args&) const -> Object      override;
        auto arity()                         const -> std::size_t override;
        auto to_string()                     const -> std::string override;
    };
} // namespace loxe


#endif // !LOXE_TREE_WALKER_CALLABLE_HPP
