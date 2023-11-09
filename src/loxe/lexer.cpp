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
    if (auto c = peek0())
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
