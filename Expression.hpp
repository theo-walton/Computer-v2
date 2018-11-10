#pragma once

#include "ExceptionTypes.hpp"
#include "ShuntingYard.hpp"
#include "Variable.hpp"

namespace Expression
{
	Variable	evaluate(const std::string& expr, const std::map<std::string, Variable>& scope);

	std::vector<RPN<Variable>>	create_rpn(const std::string& expr);

	Variable	evaluate_rpn(const std::vector<RPN<Variable>>& rpn,
				     const std::map<std::string, Variable>& scope); 
};
