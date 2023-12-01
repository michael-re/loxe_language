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
        if (match(Token::Type::Var)) return parse_var_stmt();
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
    if (match(Token::Type::For))       return parse_for_stmt();
    if (match(Token::Type::If))        return parse_if_stmt();
    if (match(Token::Type::LeftBrace)) return parse_block_stmt();
    if (match(Token::Type::Print))     return parse_print_stmt();
    if (match(Token::Type::While))     return parse_while_stmt();
    return parse_expr_stmt();
}

auto loxe::Parser::parse_block_stmt() -> ast::stmt_ptr
{
    auto statements = ast::stmt_list();
    while (!at_end() && !check(Token::Type::RightBrace))
        statements.emplace_back(parse_declaration());

    consume(Token::Type::RightBrace, "expect '}' after block");
    return ast::BlockStmt::make(std::move(statements));
}

auto loxe::Parser::parse_expr_stmt() -> ast::stmt_ptr
{
    auto expr = parse_expression();
    consume(Token::Type::Semicolon, "expect ';' after expression statement");
    return ast::ExpressionStmt::make(std::move(expr));
}

auto loxe::Parser::parse_for_stmt() -> ast::stmt_ptr
{
    consume(Token::Type::LeftParen, "expect '(' after 'for'");
    auto initializer = match(Token::Type::Semicolon) ? nullptr          :
                       match(Token::Type::Var)       ? parse_var_stmt() :
                                                       parse_expr_stmt();

    auto condition = !check(Token::Type::Semicolon) ? parse_expression() : ast::BooleanExpr::make(true);
    consume(Token::Type::Semicolon, "expect ';' after 'for' condition");
    auto update  = !check(Token::Type::RightParen) ? parse_expression() : nullptr;
    consume(Token::Type::RightParen, "expect ')' after 'for' update clauses");

    auto body    = parse_statement();
    auto loop    = ast::ForStmt::make(std::move(initializer), std::move(condition), std::move(update), std::move(body));
    auto block = ast::stmt_list();
    block.emplace_back(std::move(loop));

    return ast::BlockStmt::make(std::move(block));
}

auto loxe::Parser::parse_if_stmt() -> ast::stmt_ptr
{
    consume(Token::Type::LeftParen, "expect '(' after 'if'");
    auto condition = parse_expression();
    consume(Token::Type::RightParen, "expect ')' after 'if' condition");
    auto then_branch = parse_statement();
    auto else_branch = match(Token::Type::Else) ? parse_statement() : nullptr;
    return ast::IfStmt::make(std::move(condition), std::move(then_branch), std::move(else_branch));
}

auto loxe::Parser::parse_print_stmt() -> ast::stmt_ptr
{
    auto expr = parse_expression();
    consume(Token::Type::Semicolon, "expect ';' after print statement");
    return ast::PrintStmt::make(std::move(expr));
}

auto loxe::Parser::parse_var_stmt() -> ast::stmt_ptr
{
    auto name        = consume(Token::Type::Identifier, "expect variable name");
    auto initializer = match(Token::Type::Equal) ? parse_expression() : nullptr;
    consume(Token::Type::Semicolon, "expect ';' after variable declaration");
    return ast::VariableStmt::make(std::move(name), std::move(initializer));
}

auto loxe::Parser::parse_while_stmt() -> ast::stmt_ptr
{
    consume(Token::Type::LeftParen, "expect '(' after 'while'");
    auto condition = parse_expression();
    consume(Token::Type::RightParen, "expect ')' after 'while' condition");
    auto body = parse_statement();
    return ast::WhileStmt::make(std::move(condition), std::move(body));
}

auto loxe::Parser::parse_expression() -> ast::expr_ptr
{
    return parse_assignment();
}

auto loxe::Parser::parse_assignment() -> ast::expr_ptr
{
    auto expr = parse_logical_or();
    if (match(Token::Type::Equal))
    {
        auto equals = previous();
        auto value  = parse_assignment();
        if (auto var = dynamic_cast<ast::VariableExpr*>(expr.get()))
            return ast::AssignExpr::make(var->name, std::move(value));

        throw error(equals, "invalid assignment target");
    }

    return expr;
}

auto loxe::Parser::parse_logical_or() -> ast::expr_ptr
{
    auto expr = parse_logical_and();
    while (match(Token::Type::Or))
    {
        auto op  = previous();
        auto rhs = parse_logical_and();
        expr = ast::LogicalExpr::make(std::move(op), std::move(expr), std::move(rhs));
    }

    return expr;
}

auto loxe::Parser::parse_logical_and() -> ast::expr_ptr
{
    auto expr = parse_equality();
    while (match(Token::Type::And))
    {
        auto op  = previous();
        auto rhs = parse_equality();
        expr = ast::LogicalExpr::make(std::move(op), std::move(expr), std::move(rhs));
    }

    return expr;
}

auto loxe::Parser::parse_equality() -> ast::expr_ptr
{
    auto expr = parse_comparison();
    while (match(Token::Type::BangEqual) || match(Token::Type::EqualEqual))
    {
        auto op  = previous();
        auto rhs = parse_comparison();
        expr = ast::BinaryExpr::make(std::move(op), std::move(expr), std::move(rhs));
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
        expr = ast::BinaryExpr::make(std::move(op), std::move(expr), std::move(rhs));
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
        expr = ast::BinaryExpr::make(std::move(op), std::move(expr), std::move(rhs));
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
        expr = ast::BinaryExpr::make(std::move(op), std::move(expr), std::move(rhs));
    }

    return expr;
}

auto loxe::Parser::parse_unary() -> ast::expr_ptr
{
    while (match(Token::Type::Bang) || match(Token::Type::Minus))
    {
        auto op      = previous();
        auto operand = parse_unary();
        return ast::UnaryExpr::make(std::move(op), std::move(operand));
    }

    return parse_call();
}

auto loxe::Parser::parse_call() -> ast::expr_ptr
{
    auto expr = parse_primary();
    while (true)
    {
        if (match(Token::Type::LeftParen))
            expr = finish_call(std::move(expr));
        else
             break;
    }

    return expr;
}

auto loxe::Parser::parse_primary() -> ast::expr_ptr
{
    if (match(Token::Type::Nil))        return ast::NilExpr::make(previous());
    if (match(Token::Type::True))       return ast::BooleanExpr::make(previous());
    if (match(Token::Type::False))      return ast::BooleanExpr::make(previous());
    if (match(Token::Type::Number))     return ast::NumberExpr::make(previous());
    if (match(Token::Type::String))     return ast::StringExpr::make(previous());
    if (match(Token::Type::Identifier)) return ast::VariableExpr::make(previous());

    if (match(Token::Type::LeftParen))
    {
        auto expr = parse_expression();
        consume(Token::Type::RightParen, "expect ')' after grouping expression");
        return ast::GroupingExpr::make(std::move(expr));
    }

    throw error(current(), "expect expression");
    return {}; // unreachable
}

auto loxe::Parser::finish_call(ast::expr_ptr callee) -> ast::expr_ptr
{
    auto args = ast::expr_list();
    if (!check(Token::Type::RightParen))
    {
        do
        {
            args.emplace_back(parse_expression());
        } while (match(Token::Type::Comma));
    }

    auto paren = consume(Token::Type::RightParen, "expect ')' after arguments");
    return ast::CallExpr::make(std::move(paren), std::move(callee), std::move(args));
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
