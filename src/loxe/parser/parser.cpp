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

auto loxe::Parser::parse(std::string source) -> std::optional<ast::stmt_list>
{
    m_error = false;
    m_lexer = Lexer(std::move(source)).lex();

    auto ast = ast::stmt_list();
    while (!at_end())
    {
        if (match(Token::Type::Semicolon)) continue;
        ast.emplace_back(parse_declaration());
    }

    return m_error ? std::nullopt : std::optional(std::move(ast));
}

auto loxe::Parser::parse_declaration() -> ast::stmt_ptr
{
    try
    {
        if (match(Token::Type::Class)) return parse_class_stmt();
        if (match(Token::Type::Fun))   return parse_fun_stmt();
        if (match(Token::Type::Var))   return parse_var_stmt();
        if (match(Token::Type::Let))   return parse_let_stmt();
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
    if (match(Token::Type::Break))     return parse_break_stmt();
    if (match(Token::Type::Continue))  return parse_continue_stmt();
    if (match(Token::Type::For))       return parse_for_stmt();
    if (match(Token::Type::If))        return parse_if_stmt();
    if (match(Token::Type::LeftBrace)) return parse_block_stmt();
    if (match(Token::Type::Print))     return parse_print_stmt();
    if (match(Token::Type::Return))    return parse_return_stmt();
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

auto loxe::Parser::parse_break_stmt() -> ast::stmt_ptr
{
    auto keyword = previous();
    consume(Token::Type::Semicolon, "expect ';' after 'break'");
    return ast::BreakStmt::make(std::move(keyword));
}

auto loxe::Parser::parse_class_stmt() -> ast::stmt_ptr
{
    auto name       = consume(Token::Type::Identifier, "expect class name");
    auto superclass = ast::expr_ptr(nullptr);
    if (match(Token::Type::Less))
    {
        consume(Token::Type::Identifier, "expect superclass name");
        superclass = ast::VariableExpr::make(previous());
    }

    consume(Token::Type::LeftBrace, "expect '{' before class body");

    auto methods = ast::method_list();
    while (!check(Token::Type::RightBrace) && !at_end())
    {
        auto name   = consume(Token::Type::Identifier, "expect class method name");
        auto method = function("method");
        method->name = std::move(name);
        methods.emplace_back(std::move(method));
    }

    consume(Token::Type::RightBrace, "expect '}' after class body");
    return ast::ClassStmt::make(std::move(name), std::move(superclass), std::move(methods));
}

auto loxe::Parser::parse_continue_stmt() -> ast::stmt_ptr
{
    auto keyword = previous();
    consume(Token::Type::Semicolon, "expect ';' after 'continue'");
    return ast::ContinueStmt::make(std::move(keyword));
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

auto loxe::Parser::parse_fun_stmt() -> ast::stmt_ptr
{
    auto name = consume(Token::Type::Identifier, "expect function name");
    auto fun  = function("function");
    fun->name = std::move(name);
    return ast::FunctionStmt::make(std::move(fun));
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

auto loxe::Parser::parse_let_stmt() -> ast::stmt_ptr
{
    auto name        = consume(Token::Type::Identifier, "expect variable name after 'let'");
    consume(Token::Type::Equal, "expect '=' after 'let' variable declaration");
    auto initializer = parse_expression();
    consume(Token::Type::Semicolon, "expect ';' after `let` initializer");
    return ast::LetStmt::make(std::move(name), std::move(initializer));
}

auto loxe::Parser::parse_print_stmt() -> ast::stmt_ptr
{
    auto expr = parse_expression();
    consume(Token::Type::Semicolon, "expect ';' after print statement");
    return ast::PrintStmt::make(std::move(expr));
}

auto loxe::Parser::parse_return_stmt() -> ast::stmt_ptr
{
    auto keyword = previous();
    auto value   = !check(Token::Type::Semicolon) ? parse_expression() : nullptr;
    consume(Token::Type::Semicolon, "expect ';' after return value");
    return ast::ReturnStmt::make(std::move(keyword), std::move(value));
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
    return parse_comma();
}

auto loxe::Parser::parse_comma() -> ast::expr_ptr
{
    auto exprs = ast::expr_list();
    do
    {
        exprs.emplace_back(parse_conditional());
    } while (match(Token::Type::Comma));

    return exprs.size() == 1 ? std::move(exprs.front()) : ast::CommaExpr::make(std::move(exprs));
}

auto loxe::Parser::parse_conditional() -> ast::expr_ptr
{
    auto expr = parse_assignment();
    if (match(Token::Type::Question))
    {
        auto then_branch = parse_expression();
        consume(Token::Type::Colon, "expect ':' after then branch of conditional expression");
        auto else_branch = parse_conditional();
        return ast::ConditionalExpr::make(std::move(expr), std::move(then_branch), std::move(else_branch));
    }

    return expr;
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
        else if (auto subscript = dynamic_cast<ast::SubscriptExpr*>(expr.get()))
            return ast::SubscriptExpr::make(std::move(subscript->bracket), std::move(subscript->expression), std::move(subscript->index), std::move(value));
        else if (auto get = dynamic_cast<ast::GetExpr*>(expr.get()))
            return ast::SetExpr::make(std::move(get->name), std::move(get->object), std::move(value));

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
    while (match(Token::Type::Bang) || match(Token::Type::Minus) || match(Token::Type::Plus))
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
        {
            expr = finish_call(std::move(expr));
        }
        else if (match(Token::Type::Dot))
        {
            auto name = consume(Token::Type::Identifier, "expect property name after '.'");
            expr = ast::GetExpr::make(std::move(name), std::move(expr));
        }
        else if (match(Token::Type::LeftBracket))
        {
            auto bracket = previous();
            auto index   = parse_expression();
            consume(Token::Type::RightBracket, "expect ']' after subscript arg");
            expr = ast::SubscriptExpr::make(std::move(bracket), std::move(expr), std::move(index));
        }
        else
        {
            break;
        }
    }

    return expr;
}

auto loxe::Parser::parse_primary() -> ast::expr_ptr
{
    if (match(Token::Type::Nil))         return ast::NilExpr::make(previous());
    if (match(Token::Type::True))        return ast::BooleanExpr::make(previous());
    if (match(Token::Type::False))       return ast::BooleanExpr::make(previous());
    if (match(Token::Type::Number))      return ast::NumberExpr::make(previous());
    if (match(Token::Type::String))      return ast::StringExpr::make(previous());
    if (match(Token::Type::Identifier))  return ast::VariableExpr::make(previous());
    if (match(Token::Type::This))        return ast::ThisExpr::make(previous());
    if (match(Token::Type::Lambda))      return parse_lambda();
    if (match(Token::Type::Super))       return parse_super();
    if (match(Token::Type::LeftBrace))   return parse_array();
    if (match(Token::Type::LeftBracket)) return parse_array();
    if (match(Token::Type::LeftParen))   return parse_grouping();

    // error productions
    if (match(Token::Type::BangEqual) || match(Token::Type::EqualEqual))
    {
        auto error_production = error(previous(), "missing left-hand operand");
        utility::ignore(parse_equality());
        throw error_production;
    }

    if (match(Token::Type::BangEqual) || match(Token::Type::EqualEqual))
    {
        auto error_production = error(previous(), "missing left-hand operand");
        utility::ignore(parse_equality());
        throw error_production;
    }

    if (match(Token::Type::Greater) || match(Token::Type::GreaterEqual) ||
        match(Token::Type::Less) || match(Token::Type::LessEqual))
    {
        auto error_production = error(previous(), "missing left-hand operand");
        utility::ignore(parse_comparison());
        throw error_production;
    }

    if (match(Token::Type::Slash) || match(Token::Type::Star))
    {
        auto error_production = error(previous(), "missing left-hand operand");
        utility::ignore(parse_factor());
        throw error_production;
    }

    throw error(current(), "expect expression");
}

auto loxe::Parser::parse_lambda() -> ast::expr_ptr
{
    auto name    = previous();
    auto lambda  = function("lambda");
    lambda->name = std::move(name);
    return lambda;
}

auto loxe::Parser::parse_super() -> ast::expr_ptr
{
    auto keyword = previous();
    consume(Token::Type::Dot, "expect '.' after 'super'");
    auto method = consume(Token::Type::Identifier, "expect superclass method name");
    return ast::SuperExpr::make(std::move(keyword), std::move(method));
}

auto loxe::Parser::parse_array() -> ast::expr_ptr
{
    auto start       = previous();
    auto size        = ast::expr_ptr(nullptr);
    auto initializer = ast::expr_list();

    if (start.type == Token::Type::LeftBracket && match(Token::Type::Colon))
    {
        size = parse_expression();
        consume(Token::Type::RightBracket, "expect ']' after subscript arg");
        if (match(Token::Type::LeftBrace)) initializer = parse_initializer();
    }
    else if (start.type == Token::Type::LeftBrace)
    {
        initializer = parse_initializer();
    }

    return ast::ArrayExpr::make(std::move(start), std::move(size), std::move(initializer));
}

auto loxe::Parser::parse_grouping() -> ast::expr_ptr
{
    auto expr = parse_expression();
    consume(Token::Type::RightParen, "expect ')' after grouping expression");
    return ast::GroupingExpr::make(std::move(expr));
}

auto loxe::Parser::parse_initializer() -> ast::expr_list
{
    auto initializer = ast::expr_list();
    if (!check(Token::Type::RightBrace))
    {
        do
        {
            if (check(Token::Type::RightBrace)) break;
            initializer.emplace_back(parse_conditional());
        } while (match(Token::Type::Comma));
    }

    consume(Token::Type::RightBrace, "expect '}' after initializer expression");
    return initializer;
}

auto loxe::Parser::finish_call(ast::expr_ptr callee) -> ast::expr_ptr
{
    auto args = ast::expr_list();
    if (!check(Token::Type::RightParen))
    {
        do
        {
            args.emplace_back(parse_conditional());
        } while (match(Token::Type::Comma));
    }

    auto paren = consume(Token::Type::RightParen, "expect ')' after arguments");
    return ast::CallExpr::make(std::move(paren), std::move(callee), std::move(args));
}

auto loxe::Parser::function(const std::string& kind) -> ast::fun_ptr
{
    static const auto implicit_name = Token(Token::Type::Implicit, -1, -1, "unnamed function");
    consume(Token::Type::LeftParen, "expect '(' after " + kind + " name");
    auto params = ast::param_list();

    if (!check(Token::Type::RightParen))
    {
        do
        {
            params.emplace_back(consume(Token::Type::Identifier, "expect paramater name"));
        } while (match(Token::Type::Comma));
    }

    consume(Token::Type::RightParen, "expect ')' after parameters");
    consume(Token::Type::LeftBrace, "expect '{' before " + kind + " body");
    return ast::FunctionExpr::make(implicit_name, std::move(params), parse_block_stmt());
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
