#include "loxe/lexer.hpp"
#include "loxe/utility.hpp"

[[nodiscard]] inline constexpr auto is_digit(char c) -> bool
{
    return ('0' <= c && c <= '9');
}

[[nodiscard]] inline constexpr auto is_alpha(char c) -> bool
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

[[nodiscard]] inline constexpr auto is_ident(char c) -> bool
{
    return (c == '_' || is_digit(c) || is_alpha(c));
}

auto loxe::Lexer::lex() -> Lexer&
{
    if (m_source.empty())
    {
        return *this;
    }
    else if (m_cursor == 0)
    {
        m_cache.current = lex_token();
        m_cache.next    = lex_token();
    }
    else if (m_cache.current.type != Token::Type::EndOfFile)
    {
        m_cache.previous = std::move(m_cache.current);
        m_cache.current  = std::move(m_cache.next);
        m_cache.next     = lex_token();
    }

    return *this;
}

auto loxe::Lexer::peek_prev() const -> const Token&
{
    return m_cache.previous;
}

auto loxe::Lexer::peek_curr() const -> const Token&
{
    return m_cache.current;
}

auto loxe::Lexer::peek_next() const -> const Token&
{
    return m_cache.next;
}

auto loxe::Lexer::lex_token() -> Token
{
    const auto c = skip_whitespace();
    if      (!c)           return { Token::Type::EndOfFile, m_line, m_column };
    else if (is_digit(*c)) return lex_number();
    else if (is_ident(*c)) return lex_identifier();
    else                   return lex_punctuation();
}

auto loxe::Lexer::lex_number() -> Token
{
    const auto line   = m_line;
    const auto column = m_column;

    auto lexeme = std::string();
    for (auto c = peek0(); c && is_digit(*c); c = advance())
        lexeme.push_back(*c);

    if (peek0() && *peek0() == '.')
    {
        lexeme.push_back('.');
        for (auto c = advance(); c && is_digit(*c); c = advance())
            lexeme.push_back(*c);

        while (lexeme.back() == '0') lexeme.pop_back(); // remove trailing zeros
        if    (lexeme.back() == '.') lexeme.pop_back(); // remove trailing decimal
    }

    return { Token::Type::Number, line, column, std::move(lexeme) };
}

auto loxe::Lexer::lex_string() -> Token
{
    const auto line   = m_line;
    const auto column = m_column;

    auto lexeme = std::string();
    for (auto c = advance(); c && *c != '"'; c = advance())
        lexeme.push_back(*c);

    if (at_end())
        return { Token::Type::Unknown, line, column, "unterminated string literal" };

    utility::ignore(advance()); // trailing "
    return { Token::Type::String, line, column, std::move(lexeme) };
}

auto loxe::Lexer::lex_identifier() -> Token
{
    const auto line   = m_line;
    const auto column = m_column;

    auto lexeme = std::string();
    for (auto c = peek0(); c && is_ident(*c); c = advance())
        lexeme.push_back(*c);

    return { Token::Type::Identifier, line, column, std::move(lexeme) };
}

auto loxe::Lexer::lex_punctuation() -> Token
{
    const auto line   = m_line;
    const auto column = m_column;

    auto make_token = [&](Token::Type type, int length = 1) -> Token
    {
        auto lexeme = std::string();
        for (auto c = peek0(); c && length; length--, c = advance())
            lexeme.push_back(*c);

        return { type, line, column, std::move(lexeme) };
    };

    const auto next   = peek1();
    const auto is_eq  = next && *next == '=';
    const auto length = is_eq ? 2 : 1;

    switch (*peek0())
    {
        // single character tokens
        case '(': return make_token(Token::Type::LeftParen);
        case ')': return make_token(Token::Type::RightParen);
        case '{': return make_token(Token::Type::LeftBrace);
        case '}': return make_token(Token::Type::RightBrace);
        case ';': return make_token(Token::Type::Semicolon);
        case ',': return make_token(Token::Type::Comma);
        case '.': return make_token(Token::Type::Dot);
        case '-': return make_token(Token::Type::Minus);
        case '+': return make_token(Token::Type::Plus);
        case '/': return make_token(Token::Type::Slash);
        case '*': return make_token(Token::Type::Star);

        // single or double character tokens
        case '!': return make_token(is_eq ? Token::Type::BangEqual    : Token::Type::Bang,    length);
        case '=': return make_token(is_eq ? Token::Type::EqualEqual   : Token::Type::Equal,   length);
        case '<': return make_token(is_eq ? Token::Type::LessEqual    : Token::Type::Less,    length);
        case '>': return make_token(is_eq ? Token::Type::GreaterEqual : Token::Type::Greater, length);

        // string literals
        case '"': return lex_string();
    }

    const auto as_char = char{ *peek0() };
    const auto as_int  = int { as_char  };
    const auto value   = (as_char <= ' ') ? "int value - " + std::to_string(as_int) : std::string(1, as_char);

    utility::ignore(advance());
    return { Token::Type::Unknown, line, column, "unexpected character: (" + value + ")"};
}

auto loxe::Lexer::at_end() const -> bool
{
    return m_source.empty() || m_cursor >= m_source.size();
}

auto loxe::Lexer::peek0() const -> std::optional<char>
{
    return at_end() ? std::nullopt : std::optional(m_source[m_cursor]);
}

auto loxe::Lexer::peek1() const -> std::optional<char>
{
    const auto next = m_cursor + 1;
    return at_end() || next >= m_source.size() ? std::nullopt : std::optional(m_source[next]);
}

auto loxe::Lexer::skip_whitespace() -> std::optional<char>
{
    while (!at_end())
    {
        switch (*peek0())
        {
            case ' ':
            case '\r':
            case '\t':
            case '\n':
                utility::ignore(advance());
                break;
           case '/':
                if (auto c = peek1(); !c || (*c != '/' && *c != '*'))
                    return peek0();
                utility::ignore(skip_comment());
                break;
            default:
                return peek0();
        }
    }

    return peek0();
}

auto loxe::Lexer::skip_comment() -> std::optional<char>
{
    auto check = [&](char a, char b) -> bool
    {
        const auto c0 = peek0();
        const auto c1 = peek1();
        return (c0 && c1 && *c0 == a && *c1 == b);
    };

    if (check('/', '/')) // single-line comment
    {
        while (!at_end() && *peek0() != '\n') utility::ignore(advance());
        utility::ignore(advance()); // '\n'
    }
    else if (check('/', '*')) // multi-line comment
    {
        utility::ignore(advance(), advance()); // /*
        while (!check('*', '/')) utility::ignore(advance());
        utility::ignore(advance(), advance()); // */
    }

    return peek0();
}

auto loxe::Lexer::advance() -> std::optional<char>
{
    if (!at_end())
    {
        if (*peek0() == '\n')
        {
            m_line++;
            m_column = 0;
        }

        m_column++;
        m_cursor++;
    }

    return peek0();
}
