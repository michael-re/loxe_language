#include "loxe/tree_walker/object.hpp"

auto loxe::Object::stringify() const -> std::string
{
    switch (m_value.index())
    {
        case 0: return "nil";
        case 1: return as<boolean>() ? "true" : "false";
        case 2:
        {
            auto string = std::to_string(as<number>());
            while (string.back() == '0') string.pop_back();
            if    (string.back() == '.') string.pop_back();
            return string;
        }
        case 3: return as<string>();
        default: break;
    }

    return {};
}
