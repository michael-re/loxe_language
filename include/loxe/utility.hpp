#pragma once

#ifndef LOXE_UTILITY_HPP
#define LOXE_UTILITY_HPP

#include <format>
#include <iostream>

namespace loxe::utility
{
    template<typename... Args>
    [[maybe_unused]] inline auto print(std::ostream& stream, std::format_string<Args...> fmt, Args&&... args)
    {
        stream << std::format(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    [[maybe_unused]] inline auto println(std::ostream& stream, std::format_string<Args...> fmt, Args&&... args)
    {
        stream << std::format(fmt, std::forward<Args>(args)...) << '\n';
    }

    template<typename... Args>
    [[maybe_unused]] inline auto print(std::format_string<Args...> fmt, Args&&... args)
    {
        std::cout << std::format(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    [[maybe_unused]] inline auto println(std::format_string<Args...> fmt, Args&&... args)
    {
        std::cout << std::format(fmt, std::forward<Args>(args)...) << '\n';
    }
} // namespace loxe::utility

#endif // !LOXE_UTILITY_HPP
