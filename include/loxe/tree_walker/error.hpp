#pragma once

#ifndef LOXE_TREE_WALKER_ERROR_HPP
#define LOXE_TREE_WALKER_ERROR_HPP

#include "loxe/common/except.hpp"
#include "loxe/parser/token.hpp"

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
} // namespace loxe

#endif // !LOXE_TREE_WALKER_ERROR_HPP
