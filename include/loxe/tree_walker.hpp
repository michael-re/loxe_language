#pragma once

#ifndef LOXE_TREE_WALKER_HPP
#define LOXE_TREE_WALKER_HPP

#include <string>

#include "parser.hpp"

namespace loxe
{
    class TreeWalker
    {
    public:
        auto run     (std::string      source)   -> void;
        auto run_repl(std::string_view prompt)   -> void;
        auto run_file(std::string_view filename) -> void;

    private:
        Parser m_parser;
    };
} // namespace loxe

#endif // !LOXE_TREE_WALKER_HPP
