#pragma once

#include "ShuntingYard.hpp"
#include "Expression.hpp"
#include "Variable.hpp"
#include "Polynomial.hpp"
#include <utility>

std::string	expand_solve(const std::string& expr, const std::map<std::string, Variable>& scope);
