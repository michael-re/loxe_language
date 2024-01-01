#include <fstream>

#include "loxe/tree_walker/tree_walker.hpp"
#include "loxe/tree_walker/interpreter.hpp"

#include "loxe/common/utility.hpp"
#include "loxe/parser/parser.hpp"
#include "loxe/parser/resolver.hpp"

auto loxe::tree_walker::run_file(std::string_view filename) -> void
{
    auto file  = std::ifstream(filename.data());
    if (!file.is_open()) return utility::println(std::cerr, "[tree_walker]: failed to open file: '{}'", filename);

    auto ast = Parser().parse(utility::as_string(file.rdbuf()), std::string(filename));
    if (!ast) return utility::println(std::cerr, "[tree_walker]: encountered parser error in file '{}'", filename);

    if (Resolver().resolve_ast(*ast) == Resolver::State::Error)
        return utility::println(std::cerr, "[tree_walker]: encountered resolution error in file '{}'", filename);

    Interpreter().interpret(*ast);
}
