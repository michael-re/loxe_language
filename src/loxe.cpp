#include "loxe.hpp"

auto main() -> int
{
    using namespace loxe;
    utility::println("Hello token: {}", Token().to_string());
    return EXIT_SUCCESS;
}
