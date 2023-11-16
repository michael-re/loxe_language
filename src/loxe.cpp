#include "loxe.hpp"

auto main(int argc, char* argv[]) -> int
{
    switch (argc)
    {
        case 1:
            loxe::TreeWalker().run_repl(">> ");
            break;
        case 2:
            loxe::TreeWalker().run_file(argv[1]);
            break;
        default:
            loxe::utility::println(std::cerr, "usage: loxe [script]");
            break;
    }

    return EXIT_SUCCESS;
}
