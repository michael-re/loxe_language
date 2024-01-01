#pragma once

#ifndef LOXE_TREE_WALKER_TREE_WALKER_HPP
#define LOXE_TREE_WALKER_TREE_WALKER_HPP

#include <string>

namespace loxe::tree_walker
{
    auto run_file(std::string_view filename) -> void;
} // namespace loxe

#endif // !LOXE_TREE_WALKER_TREE_WALKER_HPP
