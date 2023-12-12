#pragma once

#ifndef LOXE_PARSER_AST_HPP
#define LOXE_PARSER_AST_HPP

#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <optional>

#include "loxe/tree_walker/object.hpp"

#include "token.hpp"

namespace loxe::ast
{
    constexpr auto is_const  = true;
    constexpr auto not_const = false;

    template<bool IsConst, typename T>
    using const_wrapper = typename std::conditional<IsConst, const T, T>::type;

    using expr_ptr = std::unique_ptr<struct Expr>;
    using stmt_ptr = std::unique_ptr<struct Stmt>;

    using expr_list = std::vector<expr_ptr>;
    using stmt_list = std::vector<stmt_ptr>;

    using fun_ptr     = std::unique_ptr<struct FunctionStmt>;
    using param_list  = std::vector<Token>;
    using method_list = std::vector<fun_ptr>;
} // namespace loxe::ast

namespace loxe::ast
{
    struct Expr
    {
    public:
        template<typename R, bool IsConst>
        struct Visitor
        {
            virtual ~Visitor() = default;
            virtual auto visit(const_wrapper<IsConst, struct AssignExpr>&)   -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct BinaryExpr>&)   -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct BooleanExpr>&)  -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct CallExpr>&)     -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct GetExpr>&)      -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct GroupingExpr>&) -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct LogicalExpr>&)  -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct NilExpr>&)      -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct NumberExpr>&)   -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct SetExpr>&)      -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct StringExpr>&)   -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct SuperExpr>&)    -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct ThisExpr>&)     -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct UnaryExpr>&)    -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct VariableExpr>&) -> R = 0;
        };

        template<typename R> using visitor       = Visitor<R, not_const>;
        template<typename R> using const_visitor = Visitor<R, is_const>;

    public:
        virtual ~Expr() = default;
        virtual auto accept(visitor<void>&)               -> void   = 0;
        virtual auto accept(const_visitor<void>&)   const -> void   = 0;
        virtual auto accept(const_visitor<Object>&) const -> Object = 0;

        [[nodiscard]] virtual auto clone() const -> expr_ptr = 0;

        std::optional<std::size_t> depth;
    };

    struct Stmt
    {
    public:
        template<typename R, bool IsConst>
        struct Visitor
        {
            virtual ~Visitor() = default;
            virtual auto visit(const_wrapper<IsConst, struct BlockStmt>&)      -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct ClassStmt>&)      -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct ExpressionStmt>&) -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct ForStmt>&)        -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct FunctionStmt>&)   -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct IfStmt>&)         -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct PrintStmt>&)      -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct ReturnStmt>&)     -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct VariableStmt>&)   -> R = 0;
            virtual auto visit(const_wrapper<IsConst, struct WhileStmt>&)      -> R = 0;
        };

        template<typename R> using visitor       = Visitor<R, not_const>;
        template<typename R> using const_visitor = Visitor<R, is_const>;

    public:
        virtual ~Stmt() = default;
        virtual auto accept(visitor<void>&)             -> void   = 0;
        virtual auto accept(const_visitor<void>&) const -> void   = 0;

        [[nodiscard]] virtual auto clone() const -> stmt_ptr = 0;
    };

    template<typename Derived>
    struct ExprCRTP : public Expr
    {
        auto accept(visitor<void>& visitor) -> void override
        {
            return visitor.visit(*static_cast<Derived*>(this));
        }

        auto accept(const_visitor<void>& visitor) const -> void override
        {
            return visitor.visit(*static_cast<const Derived*>(this));
        }

        auto accept(const_visitor<Object>& visitor) const -> Object override
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

    template<typename Derived>
    struct StmtCRTP : public Stmt
    {
        auto accept(visitor<void>& visitor) -> void override
        {
            return visitor.visit(*static_cast<Derived*>(this));
        }

        auto accept(const_visitor<void>& visitor) const -> void override
        {
            return visitor.visit(*static_cast<const Derived*>(this));
        }

        [[nodiscard]] auto clone() const -> stmt_ptr override
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
} // namespace loxe::ast

namespace loxe::ast
{
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

    struct GetExpr final : public ExprCRTP<GetExpr>
    {
        GetExpr(Token name, expr_ptr object)
            : name(std::move(name)), object(std::move(object)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<GetExpr> override
        {
            auto obj = object ? object->clone() : nullptr;
            return std::make_unique<GetExpr>(name, std::move(obj));
        }

        Token    name;
        expr_ptr object;
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

    struct SetExpr final : public ExprCRTP<SetExpr>
    {
        SetExpr(Token name, expr_ptr object, expr_ptr value)
            : name(std::move(name)), object(std::move(object)), value(std::move(value)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<SetExpr> override
        {
            auto obj = object ? object->clone() : nullptr;
            auto val = value  ? value->clone()  : nullptr;
            return std::make_unique<SetExpr>(name, std::move(obj), std::move(val));
        }

        Token    name;
        expr_ptr object;
        expr_ptr value;
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

    struct SuperExpr final : public ExprCRTP<SuperExpr>
    {
        SuperExpr(Token keyword, Token method)
            : keyword(std::move(keyword)), method(std::move(method)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<SuperExpr> override
        {
            return std::make_unique<SuperExpr>(*this);
        }

        Token keyword;
        Token method;
    };

    struct ThisExpr final : public ExprCRTP<ThisExpr>
    {
        ThisExpr(Token keyword)
            : keyword(std::move(keyword)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<ThisExpr> override
        {
            return std::make_unique<ThisExpr>(*this);
        }

        Token keyword;
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

namespace loxe::ast
{
    struct BlockStmt final : public StmtCRTP<BlockStmt>
    {
        BlockStmt(stmt_list statements)
            : statements(std::move(statements)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<BlockStmt> override
        {
            auto stmts = stmt_list();
            for (const auto& stmt : statements)
                stmts.emplace_back(stmt ? stmt->clone() : nullptr);
            return std::make_unique<BlockStmt>(std::move(stmts));
        }

        stmt_list statements;
    };

    struct ExpressionStmt final : public StmtCRTP<ExpressionStmt>
    {
        ExpressionStmt(expr_ptr expression)
            : expression(std::move(expression)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<ExpressionStmt> override
        {
            auto expr = expression ? expression->clone() : nullptr;
            return std::make_unique<ExpressionStmt>(std::move(expr));
        }

        expr_ptr expression;
    };

    struct ForStmt final : public StmtCRTP<ForStmt>
    {
        ForStmt(stmt_ptr initializer, expr_ptr condition, expr_ptr update, stmt_ptr body)
            : initializer(std::move(initializer)), condition(std::move(condition)), update(std::move(update)), body(std::move(body)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<ForStmt> override
        {
            auto init = initializer ? initializer->clone() : nullptr;
            auto con  = condition   ? condition->clone()   : nullptr;
            auto upd  = update      ? update->clone()      : nullptr;
            auto bdy  = body        ? body->clone()        : nullptr;
            return std::make_unique<ForStmt>(std::move(init), std::move(con), std::move(upd), std::move(bdy));
        }

        stmt_ptr initializer;
        expr_ptr condition;
        expr_ptr update;
        stmt_ptr body;
    };

    struct FunctionStmt final : public StmtCRTP<FunctionStmt>
    {
        FunctionStmt(Token name, param_list params, stmt_ptr body)
            : name(std::move(name)), params(std::move(params)), body(std::move(body)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<FunctionStmt> override
        {
            auto bdy = body ? body->clone() : nullptr;
            return std::make_unique<FunctionStmt>(name, params, std::move(bdy));
        }

        Token      name;
        param_list params;
        stmt_ptr   body;
    };

    struct ClassStmt final : public StmtCRTP<ClassStmt>
    {
        ClassStmt(Token name, expr_ptr superclass, method_list methods)
            : name(std::move(name)), superclass(std::move(superclass)), methods(std::move(methods)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<ClassStmt> override
        {
            auto mthds = method_list();
            auto super = superclass ? superclass->clone() : nullptr;
            for (const auto& method : methods) mthds.emplace_back(method->make_clone());
            return std::make_unique<ClassStmt>(name, std::move(super), std::move(mthds));
        }

        Token       name;
        expr_ptr    superclass;
        method_list methods;
    };

    struct IfStmt final : public StmtCRTP<IfStmt>
    {
        IfStmt(expr_ptr condition, stmt_ptr then_branch, stmt_ptr else_branch)
            : condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<IfStmt> override
        {
            auto con = condition   ? condition->clone()   : nullptr;
            auto thn = then_branch ? then_branch->clone() : nullptr;
            auto els = else_branch ? else_branch->clone() : nullptr;
            return std::make_unique<IfStmt>(std::move(con), std::move(thn), std::move(els));
        }

        expr_ptr condition;
        stmt_ptr then_branch;
        stmt_ptr else_branch;
    };

    struct PrintStmt final : public StmtCRTP<PrintStmt>
    {
        PrintStmt(expr_ptr expression)
            : expression(std::move(expression)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<PrintStmt> override
        {
            auto expr = expression ? expression->clone() : nullptr;
            return std::make_unique<PrintStmt>(std::move(expr));
        }

        expr_ptr expression;
    };

    struct ReturnStmt final : public StmtCRTP<ReturnStmt>
    {
        ReturnStmt(Token keyword, expr_ptr value)
            : keyword(std::move(keyword)), value(std::move(value)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<ReturnStmt> override
        {
            auto val = value ? value->clone() : nullptr;
            return std::make_unique<ReturnStmt>(keyword, std::move(val));
        }

        Token    keyword;
        expr_ptr value;
    };

    struct VariableStmt final : public StmtCRTP<VariableStmt>
    {
        VariableStmt(Token name, expr_ptr initializer)
            : name(std::move(name)), initializer(std::move(initializer)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<VariableStmt> override
        {
            auto init = initializer ? initializer->clone() : nullptr;
            return std::make_unique<VariableStmt>(name, std::move(init));
        }

        Token    name;
        expr_ptr initializer;
    };

    struct WhileStmt final : public StmtCRTP<WhileStmt>
    {
        WhileStmt(expr_ptr condition, stmt_ptr body)
            : condition(std::move(condition)), body(std::move(body)) {}

        [[nodiscard]] auto make_clone() const -> std::unique_ptr<WhileStmt> override
        {
            auto con = condition ? condition->clone() : nullptr;
            auto bdy = body      ? body->clone()      : nullptr;
            return std::make_unique<WhileStmt>(std::move(con), std::move(bdy));
        }

        expr_ptr condition;
        stmt_ptr body;
    };
} // namespace loxe::ast

#endif // !LOXE_PARSER_AST_HPP
