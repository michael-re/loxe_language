#include <fstream>

#include "loxe/common/utility.hpp"
#include "loxe/tree_walker/tree_walker.hpp"

auto loxe::TreeWalker::run(std::string source) -> void
{
    if (auto ast = m_parser.parse(std::move(source)))
    {
        if (!m_resolver.resolve_ast(*ast).had_error())
        {
            m_interpreter.interpret(*ast);
        }
    }
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
