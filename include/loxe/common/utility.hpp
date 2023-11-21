#pragma once

#ifndef LOXE_COMMON_UTILITY_HPP
#define LOXE_COMMON_UTILITY_HPP

#include <format>
#include <iostream>
#include <sstream>
#include <string>

namespace loxe::utility
{
    template<typename... Args>
    [[maybe_unused]] inline constexpr auto ignore(Args&&... args) -> void
    {
        return (static_cast<void>(args), ...);
    }

    template<typename T>
    [[nodiscard]] inline auto as_string(const T& value) -> std::string
    {
        return (std::ostringstream() << value).str();
    }

    template<typename... Args>
    [[nodiscard]] inline auto as_string(std::format_string<Args...> fmt, Args&&... args) -> std::string
    {
        auto buffer = std::string();
        std::format_to(std::back_inserter(buffer), fmt, std::forward<Args>(args)...);
        return buffer;
    }

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

#endif // !LOXE_COMMON_UTILITY_HPP
