#pragma once

#ifndef LOXE_EXPR_HPP
#define LOXE_EXPR_HPP

#include <memory>
#include <string>

#include "token.hpp"

namespace loxe::ast
{
    using expr_ptr = std::unique_ptr<struct Expr>;

    struct Expr
    {
        virtual ~Expr() = default;
    };

    struct BinaryExpr final : public Expr
    {
        BinaryExpr(Token op, expr_ptr lhs, expr_ptr rhs)
            : op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

        Token    op;
        expr_ptr lhs;
        expr_ptr rhs;
    };

    struct GroupingExpr final : public Expr
    {
        GroupingExpr(expr_ptr expression)
            : expression(std::move(expression)) {}

        expr_ptr expression;
    };

    struct NilExpr final : public Expr
    {
        NilExpr(Token token)
            : token(std::move(token)) {}

        Token token;
    };

    struct BooleanExpr final : public Expr
    {
        BooleanExpr(Token token)
            : value(token.lexeme == "true"), token(std::move(token)) {}

        bool  value;
        Token token;
    };

    struct NumberExpr final : public Expr
    {
        NumberExpr(Token token)
            : value(std::stod(token.lexeme)), token(std::move(token)) {}

        double value;
        Token  token;
    };

    struct StringExpr final : public Expr
    {
        StringExpr(Token value)
            : value(std::move(value)) {}

        Token value;
    };

    struct UnaryExpr final : public Expr
    {
        UnaryExpr(Token op, expr_ptr operand)
            : op(std::move(op)), operand(std::move(operand)) {}

        Token    op;
        expr_ptr operand;
    };
} // namespace loxe::ast

#endif // !LOXE_EXPR_HPP
