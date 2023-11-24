#include "loxe/common/utility.hpp"
#include "loxe/parser/parser.hpp"

loxe::Parser::ParseError::ParseError(Token token, std::string message)
    : token(std::move(token))
{
    static constexpr auto format_1 = "[{}, {}] ParseError: {} at end.";
    static constexpr auto format_2 = "[{}, {}] ParseError: {} at '{}'.";

    if (this->token.type == Token::Type::EndOfFile)
        m_message = utility::as_string(format_1, this->token.line, this->token.column, std::move(message));
    else
        m_message = utility::as_string(format_2, this->token.line, this->token.column, std::move(message), this->token.lexeme);
}

auto loxe::Parser::parse(std::string source) -> ast::stmt_list
{
    m_error = false;
    m_lexer = Lexer(std::move(source)).lex();

    auto ast = ast::stmt_list();
    while (!at_end())
    {
        if (match(Token::Type::Semicolon)) continue;
        ast.emplace_back(parse_declaration());
    }

    if (m_error)
        ast.clear();

    return ast;
}

auto loxe::Parser::parse_declaration() -> ast::stmt_ptr
{
    try
    {
        return parse_statement();
    }
    catch (const ParseError& e)
    {
        utility::println(std::cerr, "{}", e.what());
        synchronize();
        return {};
    }
}

auto loxe::Parser::parse_statement() -> ast::stmt_ptr
{
    return parse_expr_stmt();
}

auto loxe::Parser::parse_expr_stmt() -> ast::stmt_ptr
{
    auto expr = parse_expression();
    consume(Token::Type::Semicolon, "expect ';' after expression statement");
    return std::make_unique<ast::ExpressionStmt>(std::move(expr));
}

auto loxe::Parser::parse_expression() -> ast::expr_ptr
{
    return parse_equality();
}

auto loxe::Parser::parse_equality() -> ast::expr_ptr
{
    auto expr = parse_comparison();
    while (match(Token::Type::BangEqual) || match(Token::Type::EqualEqual))
    {
        auto op  = previous();
        auto rhs = parse_comparison();
        expr = std::make_unique<ast::BinaryExpr>(std::move(op), std::move(expr), std::move(rhs));
    }

    return expr;
}

auto loxe::Parser::parse_comparison() -> ast::expr_ptr
{
    auto expr = parse_term();
    while (match(Token::Type::Greater) || match(Token::Type::GreaterEqual) ||
           match(Token::Type::Less)    || match(Token::Type::LessEqual))
    {
        auto op  = previous();
        auto rhs = parse_term();
        expr = std::make_unique<ast::BinaryExpr>(std::move(op), std::move(expr), std::move(rhs));
    }

    return expr;
}

auto loxe::Parser::parse_term() -> ast::expr_ptr
{
    auto expr = parse_factor();
    while (match(Token::Type::Minus) || match(Token::Type::Plus))
    {
        auto op  = previous();
        auto rhs = parse_factor();
        expr = std::make_unique<ast::BinaryExpr>(std::move(op), std::move(expr), std::move(rhs));
    }

    return expr;
}

auto loxe::Parser::parse_factor() -> ast::expr_ptr
{
    auto expr = parse_unary();
    while (match(Token::Type::Slash) || match(Token::Type::Star))
    {
        auto op  = previous();
        auto rhs = parse_unary();
        expr = std::make_unique<ast::BinaryExpr>(std::move(op), std::move(expr), std::move(rhs));
    }

    return expr;
}

auto loxe::Parser::parse_unary() -> ast::expr_ptr
{
    while (match(Token::Type::Bang) || match(Token::Type::Minus))
    {
        auto op      = previous();
        auto operand = parse_unary();
        return std::make_unique<ast::UnaryExpr>(std::move(op), std::move(operand));
    }

    return parse_primary();
}

auto loxe::Parser::parse_primary() -> ast::expr_ptr
{
    if (match(Token::Type::Nil))    return std::make_unique<ast::NilExpr>(previous());
    if (match(Token::Type::True))   return std::make_unique<ast::BooleanExpr>(previous());
    if (match(Token::Type::False))  return std::make_unique<ast::BooleanExpr>(previous());
    if (match(Token::Type::Number)) return std::make_unique<ast::NumberExpr>(previous());
    if (match(Token::Type::String)) return std::make_unique<ast::StringExpr>(previous());

    if (match(Token::Type::LeftParen))
    {
        auto expr = parse_expression();
        consume(Token::Type::RightParen, "expect ')' after grouping expression");
        return std::make_unique<ast::GroupingExpr>(std::move(expr));
    }

    throw error(current(), "expect expression");
    return {}; // unreachable
}

auto loxe::Parser::check(Token::Type type) const -> bool
{
    return m_lexer.peek_curr().type == type;
}

auto loxe::Parser::match(Token::Type type) -> bool
{
    const auto matched = check(type);
    if (matched) utility::ignore(next());
    return matched;
}

auto loxe::Parser::at_end() const -> bool
{
    return check(Token::Type::EndOfFile);
}

auto loxe::Parser::current() const -> Token
{
    return m_lexer.peek_curr();
}

auto loxe::Parser::previous() const -> Token
{
    return m_lexer.peek_prev();
}

auto loxe::Parser::next() -> Token
{
    return m_lexer.lex().peek_prev();
}

auto loxe::Parser::consume(Token::Type type, std::string msg) -> Token
{
    if (check(type)) return next();
    throw error(current(), std::move(msg));
    return {}; // unreachable
}

auto loxe::Parser::error(Token token, std::string msg) -> ParseError
{
    m_error = true;
    return ParseError(std::move(token), std::move(msg));
}

auto loxe::Parser::synchronize() -> void
{
    utility::ignore(next());
    while (!at_end() && previous().type != Token::Type::Semicolon)
    {
        switch (current().type)
        {
            case Token::Type::Class:
            case Token::Type::Fun:
            case Token::Type::Var:
            case Token::Type::If:
            case Token::Type::For:
            case Token::Type::While:
            case Token::Type::Print:
            case Token::Type::Return: return;
            default: utility::ignore(next());
        }
    }
}
