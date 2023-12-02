#pragma once

#ifndef LOXE_LOXE_HPP
#define LOXE_LOXE_HPP

// common
#include "loxe/common/except.hpp"
#include "loxe/common/utility.hpp"

// parser
#include "loxe/parser/expr.hpp"
#include "loxe/parser/lexer.hpp"
#include "loxe/parser/parser.hpp"
#include "loxe/parser/resolver.hpp"
#include "loxe/parser/stmt.hpp"
#include "loxe/parser/token.hpp"

// tree walker
#include "loxe/tree_walker/interpreter.hpp"
#include "loxe/tree_walker/object.hpp"
#include "loxe/tree_walker/tree_walker.hpp"

#endif // !LOXE_LOXE_HPP
