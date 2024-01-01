#include "loxe/common/utility.hpp"
#include "loxe/tree_walker/error.hpp"
#include "loxe/tree_walker/callable.hpp"
#include "loxe/tree_walker/instance.hpp"
#include "loxe/tree_walker/environment.hpp"
#include "loxe/tree_walker/interpreter.hpp"

loxe::Interpreter::Interpreter()
    : m_global(std::make_shared<Environment>()), m_environment(m_global) {}

auto loxe::Interpreter::interpret(const ast::stmt_list& program) -> void
{
    try
    {
        for (const auto& stmt : program)
            execute(stmt);
    }
    catch (const RuntimeError& e)
    {
        utility::println(std::cerr, "{}", e.what());
    }
}

auto loxe::Interpreter::evaluate(const ast::expr_ptr& expr) -> Object
{
    return expr ? expr->accept(*this) : Object();
}

auto loxe::Interpreter::execute(const ast::stmt_ptr& stmt) -> void
{
    if (stmt) stmt->accept(*this);
}

auto loxe::Interpreter::execute(const ast::stmt_list& stmts, env_ptr env) -> void
{
    auto previous = std::move(m_environment);
    try
    {
        m_environment = std::move(env);
        for (const auto& stmt : stmts)
            execute(stmt);

        m_environment = std::move(previous);
    }
    catch (const BreakError& e)
    {
        m_environment = std::move(previous);
        throw e;
    }
    catch (const ContinueError& e)
    {
        m_environment = std::move(previous);
        throw e;
    }
    catch (ReturnError& e)
    {
        m_environment = std::move(previous);
        throw e;
    }
    catch (const RuntimeError& e)
    {
        m_environment = std::move(previous);
        throw e;
    }
}

auto loxe::Interpreter::visit(const ast::BlockStmt& stmt) -> void
{
    execute(stmt.statements, std::make_shared<Environment>(m_environment.get()));
}

auto loxe::Interpreter::visit(const ast::BreakStmt& stmt) -> void
{
    utility::ignore(stmt);
    throw BreakError();
}

auto loxe::Interpreter::visit(const ast::ClassStmt& stmt) -> void
{
    static const auto implicit_super = Token(Token::Type::Implicit, -1, -1, "super");

    auto super = ClassObj::super_type(nullptr);
    if (stmt.superclass)
    {
        auto superclass  = evaluate(stmt.superclass);
        auto is_callable = superclass.is<Object::callable>();
        auto as_callable = is_callable ? superclass.as<Object::callable>() : nullptr;
        super = std::dynamic_pointer_cast<ClassObj>(as_callable);
        if (!super)
        {
            auto super_expr = dynamic_cast<ast::VariableExpr*>(stmt.superclass.get());
            throw RuntimeError(super_expr->name, "superclass must be a class");
        }
    }

    auto methods     = ClassObj::methods_type();
    auto environment = std::make_shared<Environment>(m_environment.get());
    if (super) environment->define(implicit_super, { super });

    for (const auto& method : stmt.methods)
    {
        auto dec  = method->make_clone();
        auto init = method->name.lexeme == "init";
        methods[method->name.lexeme] = std::make_shared<FunctionObj>(std::move(dec), environment, init);
    }

    auto class_dec = std::make_shared<ClassObj>(stmt.name, std::move(methods), std::move(super));
    m_environment->define(stmt.name, { std::move(class_dec) });
}

auto loxe::Interpreter::visit(const ast::ContinueStmt& stmt) -> void
{
    utility::ignore(stmt);
    throw ContinueError();
}

auto loxe::Interpreter::visit(const ast::ExpressionStmt& stmt) -> void
{
    evaluate(stmt.expression);
}

auto loxe::Interpreter::visit(const ast::ForStmt& stmt) -> void
{
    for (execute(stmt.initializer); evaluate(stmt.condition).is_truthy(); evaluate(stmt.update))
    {
        try
        {
            execute(stmt.body);
        }
        catch (const BreakError&)
        {
            break;
        }
        catch (const ContinueError&)
        {
            continue;
        }
    }
}

auto loxe::Interpreter::visit(const ast::FunctionStmt& stmt) -> void
{
    auto closure  = std::make_shared<Environment>(m_environment.get());
    auto function = std::make_shared<FunctionObj>(stmt.function->make_clone(), std::move(closure));
    m_environment->define(stmt.function->name, { std::move(function) });
}

auto loxe::Interpreter::visit(const ast::IfStmt& stmt) -> void
{
    if (evaluate(stmt.condition).is_truthy())
        execute(stmt.then_branch);
    else if (stmt.else_branch)
        execute(stmt.else_branch);
}

auto loxe::Interpreter::visit(const ast::ImportStmt& stmt) -> void
{
    for (const auto& dec : stmt.body)
        execute(dec);
}

auto loxe::Interpreter::visit(const ast::LetStmt& stmt) -> void
{
    m_environment->define(stmt.name, evaluate(stmt.initializer));
}

auto loxe::Interpreter::visit(const ast::ModuleStmt& stmt) -> void
{
    auto previous = std::move(m_environment);
    m_environment = std::make_shared<Environment>(previous.get());

    for (const auto& dec : stmt.body)
        execute(dec);

    auto module_env = std::move(m_environment);
    m_environment   = std::move(previous);
    auto new_module = std::make_shared<Object::Module>(stmt.name.lexeme, std::move(module_env));
    m_environment->define(stmt.name, { std::move(new_module) });
}

auto loxe::Interpreter::visit(const ast::PrintStmt& stmt) -> void
{
    utility::println("{}", evaluate(stmt.expression).stringify());
}

auto loxe::Interpreter::visit(const ast::ReturnStmt& stmt) -> void
{
    throw ReturnError(evaluate(stmt.value));
}

auto loxe::Interpreter::visit(const ast::VariableStmt& stmt) -> void
{
    m_environment->define(stmt.name, evaluate(stmt.initializer));
}

auto loxe::Interpreter::visit(const ast::WhileStmt& stmt) -> void
{
    while (evaluate(stmt.condition).is_truthy())
    {
        try
        {
            execute(stmt.body);
        }
        catch (const BreakError&)
        {
            break;
        }
        catch (const ContinueError&)
        {
            continue;
        }
    }
}

auto loxe::Interpreter::visit(const ast::ArrayExpr& expr) -> Object
{
    auto values = Object::Array::container();
    for (const auto& value : expr.initializer)
        values.emplace_back(evaluate(value));

    if (auto size = evaluate(expr.size); expr.size)
    {
        if (!size.is<Object::number>())
            throw RuntimeError(expr.start, "array size must be a number during initialization");

        auto as_number = static_cast<std::size_t>(size.as<Object::number>());
        if (values.size() < as_number) values.resize(as_number, Object());
    }

    return { std::make_shared<Object::Array>(std::move(values)) };
}

auto loxe::Interpreter::visit(const ast::AssignExpr& expr) -> Object
{
    return m_environment->assign_at(*expr.depth, expr.name, evaluate(expr.value));
}

auto loxe::Interpreter::visit(const ast::BinaryExpr& expr) -> Object
{
    auto number = [&expr](const Object& object) -> Object::number {
        if (!object.is<Object::number>())
            throw RuntimeError(expr.op, "operator requires a number");
        return object.as<Object::number>();
    };

    auto lhs = evaluate(expr.lhs);
    auto rhs = evaluate(expr.rhs);

    switch (expr.op.type)
    {
        case Token::Type::BangEqual:  return { lhs != rhs };
        case Token::Type::EqualEqual: return { lhs == rhs };
        case Token::Type::Plus:
            if (lhs.is<Object::string>() && rhs.is<Object::string>())
                return { lhs.as<Object::string>() + rhs.as<Object::string>() };
            if (lhs.is<Object::number>() && rhs.is<Object::number>())
                return { lhs.as<Object::number>() + rhs.as<Object::number>() };
            throw RuntimeError(expr.op, "'+' operator requires two numbers or strings");
            break;
        case Token::Type::Minus:        return { number(lhs) -  number(rhs) };
        case Token::Type::Star:         return { number(lhs) *  number(rhs) };
        case Token::Type::Slash:        return { number(lhs) /  number(rhs) };
        case Token::Type::Greater:      return { number(lhs) >  number(rhs) };
        case Token::Type::GreaterEqual: return { number(lhs) >= number(rhs) };
        case Token::Type::Less:         return { number(lhs) <  number(rhs) };
        case Token::Type::LessEqual:    return { number(lhs) <= number(rhs) };
        default: throw RuntimeError(expr.op, "unrecognized binary operator: '" + expr.op.lexeme + "'");
    }
}

auto loxe::Interpreter::visit(const ast::BooleanExpr& expr) -> Object
{
    return { expr.value };
}

auto loxe::Interpreter::visit(const ast::CallExpr& expr) -> Object
{
    auto callee = evaluate(expr.callee);
    auto args   = Callable::args();

    args.reserve(expr.args.size());
    for (const auto& arg : expr.args)
        args.push_back(evaluate(arg));

    if (!callee.is<Object::callable>())
        throw RuntimeError(expr.paren, "can only call functions and classes");

    auto &callable = callee.as<Object::callable>();
    if (callable->arity() != args.size())
        throw RuntimeError(expr.paren, utility::as_string("expected {} args but got {}", callable-> arity(), args.size()));

    return callable->call(*this, std::move(args));
}

auto loxe::Interpreter::visit(const ast::CommaExpr& expr) -> Object
{
    for (auto it = expr.expressions.begin() + 1; it < expr.expressions.end(); it++)
        evaluate(*it);

    return evaluate(expr.expressions.back());
}

auto loxe::Interpreter::visit(const ast::ConditionalExpr& expr) -> Object
{
    auto condition = evaluate(expr.condition);
    return condition.is_truthy() ? evaluate(expr.then_branch) : evaluate(expr.else_branch);
}

auto loxe::Interpreter::visit(const ast::FunctionExpr& expr) -> Object
{
    auto closure  = std::make_shared<Environment>(m_environment.get());
    auto function = std::make_shared<FunctionObj>(expr.make_clone(), std::move(closure));
    return { std::move(function) };
}

auto loxe::Interpreter::visit(const ast::GetExpr& expr) -> Object
{
    if (auto value = evaluate(expr.object); value.is<Object::instance>())
        return value.as<Object::instance>()->get(expr.name);
    else if (value.is<Object::array>() && expr.name.lexeme == "length")
        return static_cast<Object::number>(value.as<Object::array>()->length());
    else if (value.is<Object::module_>())
        return value.as<Object::module_>()->env()->access(expr.name);

    throw RuntimeError(expr.name, "only instance have properties");
}

auto loxe::Interpreter::visit(const ast::GroupingExpr& expr) -> Object
{
    return evaluate(expr.expression);
}

auto loxe::Interpreter::visit(const ast::LogicalExpr& expr) -> Object
{
    auto lhs = evaluate(expr.lhs);
    if (expr.op.type == Token::Type::Or  && lhs.is_truthy())  return lhs;
    if (expr.op.type == Token::Type::And && !lhs.is_truthy()) return lhs;
    return evaluate(expr.rhs);
}

auto loxe::Interpreter::visit(const ast::NilExpr& expr) -> Object
{
    utility::ignore(expr);
    return {};
}

auto loxe::Interpreter::visit(const ast::NumberExpr& expr) -> Object
{
    return { expr.value };
}

auto loxe::Interpreter::visit(const ast::SetExpr& expr) -> Object
{
    if (auto object = evaluate(expr.object); object.is<Object::instance>())
        return object.as<Object::instance>()->set(expr.name, evaluate(expr.value));
    else if (object.is<Object::module_>())
        return object.as<Object::module_>()->env()->assign(expr.name, evaluate(expr.value));
    throw RuntimeError(expr.name, "only instances have properties");
}

auto loxe::Interpreter::visit(const ast::StringExpr& expr) -> Object
{
    return { expr.value };
}

auto loxe::Interpreter::visit(const ast::SubscriptExpr& expr) -> Object
{
    auto object = evaluate(expr.expression);
    if (!object.is<Object::array>())
        throw RuntimeError(expr.bracket, "subscript operator can only be used on arrays");

    auto array = object.as<Object::array>();
    auto index = evaluate(expr.index);
    if (!index.is<Object::number>())
        throw RuntimeError(expr.bracket, "subscript operator requires a number operator");

    auto index_value = index.as<Object::number>();
    if (static_cast<std::size_t>(index_value) > array->length() || index_value < 0)
        throw RuntimeError(expr.bracket, "index out of range for array");

    if (expr.new_value)
        return array->assign_at(index_value, evaluate(*expr.new_value));

    return array->access_at(index_value);
}

auto loxe::Interpreter::visit(const ast::SuperExpr& expr) -> Object
{
    auto distance   = *expr.depth;
    auto superclass = m_environment->access_at(distance, expr.keyword);
    auto object     = m_environment->access_at(distance - 1, expr.method);
    auto as_class   = std::dynamic_pointer_cast<ClassObj>(superclass.as<Object::callable>());

    if (auto method = as_class->find_method(expr.method))
        return method->bind(object.as<Object::instance>());
    throw RuntimeError(expr.method, "undefined property '" + expr.method.lexeme + "'");
}

auto loxe::Interpreter::visit(const ast::ThisExpr& expr) -> Object
{
    return look_up_var(expr.keyword, expr);
}

auto loxe::Interpreter::visit(const ast::UnaryExpr& expr) -> Object
{
    switch (auto operand = evaluate(expr.operand); expr.op.type)
    {
        case  Token::Type::Plus:
            if (!operand.is<Object::number>())
                throw RuntimeError(expr.op, "'+' unary operator requires a numbers");
            return { +operand.as<Object::number>() };
        case Token::Type::Minus:
            if (!operand.is<Object::number>())
                throw RuntimeError(expr.op, "'-' unary operator requires a numbers");
            return { -operand.as<Object::number>() };
        case Token::Type::Bang:
            return { !operand.is_truthy() };
        default:
            throw RuntimeError(expr.op, "invalid unary operator");
    }
}

auto loxe::Interpreter::visit(const ast::VariableExpr& expr) -> Object
{
    return look_up_var(expr.name, expr);
}

auto loxe::Interpreter::look_up_var(const Token& name, const ast::Expr& expr) -> const Object&
{
    return expr.depth ? m_environment->access_at(*expr.depth, name) : m_global->access(name);
}
