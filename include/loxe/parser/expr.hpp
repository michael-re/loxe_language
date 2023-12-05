#pragma once

#ifndef LOXE_PARSER_EXPR_HPP
#define LOXE_PARSER_EXPR_HPP

#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <optional>

#include "token.hpp"

namespace loxe::ast
{
    using expr_ptr  = std::unique_ptr<struct Expr>;
    using expr_list = std::vector<expr_ptr>;

    struct Expr
    {
        struct Visitor
        {
            virtual ~Visitor() = default;
            virtual auto visit(struct AssignExpr&)   -> void = 0;
            virtual auto visit(struct BinaryExpr&)   -> void = 0;
            virtual auto visit(struct BooleanExpr&)  -> void = 0;
            virtual auto visit(struct CallExpr&)     -> void = 0;
            virtual auto visit(struct GroupingExpr&) -> void = 0;
            virtual auto visit(struct LogicalExpr&)  -> void = 0;
            virtual auto visit(struct NilExpr&)      -> void = 0;
            virtual auto visit(struct NumberExpr&)   -> void = 0;
            virtual auto visit(struct StringExpr&)   -> void = 0;
            virtual auto visit(struct UnaryExpr&)    -> void = 0;
            virtual auto visit(struct VariableExpr&) -> void = 0;
        };

        struct ConstVisitor
        {
            virtual ~ConstVisitor() = default;
            virtual auto visit(const struct AssignExpr&)   -> void = 0;
            virtual auto visit(const struct BinaryExpr&)   -> void = 0;
            virtual auto visit(const struct BooleanExpr&)  -> void = 0;
            virtual auto visit(const struct CallExpr&)     -> void = 0;
            virtual auto visit(const struct GroupingExpr&) -> void = 0;
            virtual auto visit(const struct LogicalExpr&)  -> void = 0;
            virtual auto visit(const struct NilExpr&)      -> void = 0;
            virtual auto visit(const struct NumberExpr&)   -> void = 0;
            virtual auto visit(const struct StringExpr&)   -> void = 0;
            virtual auto visit(const struct UnaryExpr&)    -> void = 0;
            virtual auto visit(const struct VariableExpr&) -> void = 0;
        };

        virtual ~Expr() = default;
        virtual auto accept(Visitor&)            -> void = 0;
        virtual auto accept(ConstVisitor&) const -> void = 0;

        [[nodiscard]] virtual auto clone() const -> expr_ptr = 0;

        std::optional<std::size_t> depth;
    };

    template<typename Derived>
    struct ExprCRTP : public Expr
    {
        auto accept(Visitor& visitor) -> void override
        {
            return visitor.visit(*static_cast<Derived*>(this));
        }

        auto accept(ConstVisitor& visitor) const -> void override
        {
            return visitor.visit(*static_cast<const Derived*>(this));
        }

        [[nodiscard]] auto clone() const -> expr_ptr override
        {
            return this->make_clone();
        }

        template<typename... Args>
        [[nodiscard]] static auto make(Args&&... args) -> std::unique_ptr<Derived>
        {
            return std::make_unique<Derived>(std::forward<Args>(args)...);
        }

    protected:
        [[nodiscard]] virtual auto make_clone() const -> std::unique_ptr<Derived> = 0;
    };

    struct AssignExpr final : public ExprCRTP<AssignExpr>
    {
        AssignExpr(Token name, expr_ptr value)
            : name(std::move(name)), value(std::move(value)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<AssignExpr> override
        {
            auto val = value ? value->clone() : nullptr;
            return std::make_unique<AssignExpr>(name, std::move(val));
        }

        Token    name;
        expr_ptr value;
    };

    struct BinaryExpr final : public ExprCRTP<BinaryExpr>
    {
        BinaryExpr(Token op, expr_ptr lhs, expr_ptr rhs)
            : op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<BinaryExpr> override
        {
            auto left  = lhs ? lhs->clone() : nullptr;
            auto right = rhs ? rhs->clone() : nullptr;
            return std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
        }

        Token    op;
        expr_ptr lhs;
        expr_ptr rhs;
    };

    struct BooleanExpr final : public ExprCRTP<BooleanExpr>
    {
        BooleanExpr(Token token)
            : value(token.type == Token::Type::True), token(std::move(token)) {}

        BooleanExpr(bool value)
            : value(value), token() {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<BooleanExpr> override
        {
            return std::make_unique<BooleanExpr>(*this);
        }

        bool  value;
        Token token;
    };

    struct CallExpr final : public ExprCRTP<CallExpr>
    {
        CallExpr(Token paren, expr_ptr callee, expr_list args)
            : paren(std::move(paren)), callee(std::move(callee)), args(std::move(args)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<CallExpr> override
        {
            auto cal = callee ? callee->clone() : nullptr;
            auto ags = expr_list();
            for (const auto& arg : args) ags.emplace_back(arg ? arg->clone() : nullptr);
            return std::make_unique<CallExpr>(paren, std::move(cal), std::move(ags));
        }

        Token     paren;
        expr_ptr  callee;
        expr_list args;
    };

    struct GroupingExpr final : public ExprCRTP<GroupingExpr>
    {
        GroupingExpr(expr_ptr expression)
            : expression(std::move(expression)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<GroupingExpr> override
        {
            auto expr = expression ? expression->clone() : nullptr;
            return std::make_unique<GroupingExpr>(std::move(expr));
        }

        expr_ptr expression;
    };

    struct LogicalExpr final : public ExprCRTP<LogicalExpr>
    {
        LogicalExpr(Token op, expr_ptr lhs, expr_ptr rhs)
            : op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<LogicalExpr> override
        {
            auto left  = lhs ? lhs->clone() : nullptr;
            auto right = rhs ? rhs->clone() : nullptr;
            return std::make_unique<LogicalExpr>(op, std::move(left), std::move(right));
        }

        Token    op;
        expr_ptr lhs;
        expr_ptr rhs;
    };

    struct NilExpr final : public ExprCRTP<NilExpr>
    {
        NilExpr()
            : token() {}

        NilExpr(Token token)
            : token(std::move(token)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<NilExpr> override
        {
            return std::make_unique<NilExpr>(*this);
        }

        Token token;
    };

    struct NumberExpr final : public ExprCRTP<NumberExpr>
    {
        NumberExpr(Token token)
            : value(std::stod(token.lexeme)), token(std::move(token)) {}

        NumberExpr(double value)
            : value(value), token() {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<NumberExpr> override
        {
            return std::make_unique<NumberExpr>(*this);
        }

        double value;
        Token  token;
    };

    struct StringExpr final : public ExprCRTP<StringExpr>
    {
        StringExpr(Token token)
            : value(token.lexeme), token(std::move(token)) {}

        StringExpr(std::string value)
            : value(std::move(value)), token() {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<StringExpr> override
        {
            return std::make_unique<StringExpr>(*this);
        }

        std::string value;
        Token       token;
    };

    struct UnaryExpr final : public ExprCRTP<UnaryExpr>
    {
        UnaryExpr(Token op, expr_ptr operand)
            : op(std::move(op)), operand(std::move(operand)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<UnaryExpr> override
        {
            auto oper = operand ? operand->clone() : nullptr;
            return std::make_unique<UnaryExpr>(op, std::move(oper));
        }

        Token    op;
        expr_ptr operand;
    };

    struct VariableExpr final : public ExprCRTP<VariableExpr>
    {
        VariableExpr(Token name)
            : name(std::move(name)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<VariableExpr> override
        {
            return std::make_unique<VariableExpr>(*this);
        }

        Token name;
    };
} // namespace loxe::ast

#endif // !LOXE_PARSER_EXPR_HPP
