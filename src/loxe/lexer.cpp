#include "loxe/lexer.hpp"
#include "loxe/utility.hpp"

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
    if (const auto c = skip_whitespace())
    {
        auto line = m_line;
        auto column = m_column;
        utility::ignore(advance());
        return { Token::Type::Unknown, line, column, { *c } };
    }

    return { Token::Type::EndOfFile, m_line, m_column };
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
            m_column = 1;
        }
        else
        {
            m_column++;
        }

        m_cursor++;
    }
    return peek0();
}
