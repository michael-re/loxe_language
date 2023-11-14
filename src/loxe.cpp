#include "loxe.hpp"

auto main() -> int
{
    using namespace loxe;

    auto line = std::string();
    std::getline(std::cin, line);

    auto expression = Parser().parse(std::move(line));
    if (expression) utility::println("expression parsed successfully");
    else            utility::println("error parsing expression");

    return EXIT_SUCCESS;
}
