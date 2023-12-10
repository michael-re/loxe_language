#pragma once

#ifndef LOXE_LOXE_HPP
#define LOXE_LOXE_HPP

// common
#include "loxe/common/except.hpp"
#include "loxe/common/utility.hpp"

// parser
#include "loxe/parser/ast.hpp"
#include "loxe/parser/lexer.hpp"
#include "loxe/parser/parser.hpp"
#include "loxe/parser/resolver.hpp"
#include "loxe/parser/token.hpp"

// tree walker
#include "loxe/tree_walker/callable.hpp"
#include "loxe/tree_walker/environment.hpp"
#include "loxe/tree_walker/error.hpp"
#include "loxe/tree_walker/instance.hpp"
#include "loxe/tree_walker/interpreter.hpp"
#include "loxe/tree_walker/object.hpp"
#include "loxe/tree_walker/tree_walker.hpp"

#endif // !LOXE_LOXE_HPP
