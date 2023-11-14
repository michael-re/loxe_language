#pragma once

#ifndef LOXE_EXCEPT_HPP
#define LOXE_EXCEPT_HPP

#include <string>
#include <stdexcept>

namespace loxe
{
    class Exception : public std::exception
    {
    public:
        Exception(std::string message = {})
            : m_message(std::move(message)) {}

        [[nodiscard]] auto what() const noexcept -> const char* override
        {
            return m_message.c_str();
        }

    protected:
        std::string m_message;
    };
} // namespace loxe

#endif // !LOXE_EXCEPT_HPP
