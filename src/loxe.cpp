#include "loxe.hpp"

auto main() -> int
{
    using namespace loxe;

    auto line = std::string();
    std::getline(std::cin, line);

    auto lexer = Lexer(std::move(line));
    while (lexer.lex().peek_curr().type != Token::Type::EndOfFile)
        utility::println("{}", lexer.peek_curr().to_string());

    return EXIT_SUCCESS;
}
