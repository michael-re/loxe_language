#include <chrono>

#include "loxe/tree_walker/callable.hpp"

auto loxe::NativeClock::call(Interpreter&, const args&) const -> Object
{
    const auto time = std::chrono::high_resolution_clock::now().time_since_epoch();
    const auto ms   = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
    return { static_cast<Object::number>(ms) };
}

auto loxe::NativeClock::arity() const -> std::size_t
{
    return 0;
}

auto loxe::NativeClock::to_string() const -> std::string
{
    return "<fn native clock>";
}
