#pragma once

#include <string>
#include <algorithm>
#include <iostream>
#include "Expression.hpp"
#include "MonoFunction.hpp"
#include "solve.hpp"

class	Interpreter
{
	std::map<std::string, Variable> _variables;
	std::map<std::string, MonoFunction> _functions;

	static std::string	normalizeWhitespace(const std::string&);

	static std::string	getVar(const std::string& line, std::string* arg);

	static bool		isQuestion(const std::string&);
	static bool		validExpr(const std::string&);
	static std::string	getExpr(const std::string&);

public:
	void	ReadLine(const std::string&);
};
