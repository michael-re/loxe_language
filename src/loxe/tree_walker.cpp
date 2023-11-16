#include <fstream>

#include "loxe/tree_walker.hpp"
#include "loxe/utility.hpp"

auto loxe::TreeWalker::run(std::string source) -> void
{
    auto program = m_parser.parse(std::move(source));
    if (program) utility::println("program parsed successfully");
    else         utility::println("error parsing program");
}

auto loxe::TreeWalker::run_repl(std::string_view prompt) -> void
{
    while (true)
    {
        utility::print("{}", prompt);
        auto line = std::string();
        std::getline(std::cin, line);
        if (line.empty()) break;
        run(std::move(line));
    }
}

auto loxe::TreeWalker::run_file(std::string_view filename) -> void
{
    if (const auto file = std::ifstream(filename.data()); file.is_open())
        return run(utility::as_string(file.rdbuf()));
    else
        utility::println(std::cerr, "failed to open file: '{}'", filename);
}