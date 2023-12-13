#pragma once

#ifndef LOXE_TREE_WALKER_ERROR_HPP
#define LOXE_TREE_WALKER_ERROR_HPP

#include "loxe/common/utility.hpp"
#include "loxe/common/except.hpp"
#include "loxe/parser/token.hpp"

#include "object.hpp"

namespace loxe
{
    struct RuntimeError : public Exception
    {
        RuntimeError(const Token& token, std::string message)
        {
            static constexpr auto format = "[{}, {}] RuntimeError at token '{}': {}.";
            m_message = utility::as_string(format, token.line, token.column, token.lexeme, std::move(message));
        }
    };

    struct ReturnError : public Exception
    {
        ReturnError(Object value)
            : Exception("using exception to return value"), value(std::move(value)) {}

        Object value;
    };

    struct BreakError : public Exception
    {
        BreakError()
            : Exception("using exception to break from loop") {}
    };
} // namespace loxe

#endif // !LOXE_TREE_WALKER_ERROR_HPP
