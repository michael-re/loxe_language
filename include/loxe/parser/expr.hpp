#pragma once

#ifndef LOXE_PARSER_EXPR_HPP
#define LOXE_PARSER_EXPR_HPP

#include <memory>
#include <string>

#include "token.hpp"

namespace loxe::ast
{
    using expr_ptr = std::unique_ptr<struct Expr>;

    struct Expr
    {
        struct Visitor
        {
            virtual ~Visitor() = default;
            virtual auto visit(const struct BinaryExpr&)   -> void = 0;
            virtual auto visit(const struct BooleanExpr&)  -> void = 0;
            virtual auto visit(const struct GroupingExpr&) -> void = 0;
            virtual auto visit(const struct NilExpr&)      -> void = 0;
            virtual auto visit(const struct NumberExpr&)   -> void = 0;
            virtual auto visit(const struct StringExpr&)   -> void = 0;
            virtual auto visit(const struct UnaryExpr&)    -> void = 0;
        };

        virtual ~Expr() = default;
        virtual auto accept(Visitor&) const -> void = 0;
    };

    template<typename Derived>
    struct ExprCRTP : public Expr
    {
        auto accept(Visitor& visitor) const -> void override
        {
            return visitor.visit(*static_cast<const Derived*>(this));
        };

        template<typename... Args>
        [[nodiscard]] static auto make(Args&&... args) -> expr_ptr
        {
            return std::make_unique<Derived>(std::forward<Args>(args)...);
        }
    };

    struct BinaryExpr final : public ExprCRTP<BinaryExpr>
    {
        BinaryExpr(Token op, expr_ptr lhs, expr_ptr rhs)
            : op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

        Token    op;
        expr_ptr lhs;
        expr_ptr rhs;
    };

    struct BooleanExpr final : public ExprCRTP<BooleanExpr>
    {
        BooleanExpr(Token token)
            : value(token.type == Token::Type::True), token(std::move(token)) {}

        bool  value;
        Token token;
    };

    struct GroupingExpr final : public ExprCRTP<GroupingExpr>
    {
        GroupingExpr(expr_ptr expression)
            : expression(std::move(expression)) {}

        expr_ptr expression;
    };

    struct NilExpr final : public ExprCRTP<NilExpr>
    {
        NilExpr(Token token)
            : token(std::move(token)) {}

        Token token;
    };

    struct NumberExpr final : public ExprCRTP<NumberExpr>
    {
        NumberExpr(Token token)
            : value(std::stod(token.lexeme)), token(std::move(token)) {}

        double value;
        Token  token;
    };

    struct StringExpr final : public ExprCRTP<StringExpr>
    {
        StringExpr(Token value)
            : value(std::move(value)) {}

        Token value;
    };

    struct UnaryExpr final : public ExprCRTP<UnaryExpr>
    {
        UnaryExpr(Token op, expr_ptr operand)
            : op(std::move(op)), operand(std::move(operand)) {}

        Token    op;
        expr_ptr operand;
    };
} // namespace loxe::ast

#endif // !LOXE_PARSER_EXPR_HPP
