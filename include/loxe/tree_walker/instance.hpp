#pragma once

#ifndef LOXE_TREE_WALKER_INSTANCE_HPP
#define LOXE_TREE_WALKER_INSTANCE_HPP

#include <string>
#include <memory>
#include <unordered_map>

#include "object.hpp"
#include "callable.hpp"

namespace loxe
{
    class InstanceObj : public std::enable_shared_from_this<InstanceObj>
    {
    public:
        using class_type = ClassObj;
        using field_type = std::unordered_map<std::string, Object>;

    public:
        InstanceObj(class_type class_obj)
            : m_class(std::move(class_obj)), m_fields({}) {}

        auto to_string() const -> std::string;

        auto get(const class Token& name)               -> Object;
        auto set(const class Token& name, Object value) -> Object&;

    private:
        class_type m_class;
        field_type m_fields;
    };
} // namespace loxe


#endif // LOXE_TREE_WALKER_INSTANCE_HPP
