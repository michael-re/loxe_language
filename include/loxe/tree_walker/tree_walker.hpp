#pragma once

#ifndef LOXE_TREE_WALKER_TREE_WALKER_HPP
#define LOXE_TREE_WALKER_TREE_WALKER_HPP

#include <string>

namespace loxe::tree_walker
{
    auto run_file(std::string filename) -> void;
} // namespace loxe::tree_walker

#endif // !LOXE_TREE_WALKER_TREE_WALKER_HPP
