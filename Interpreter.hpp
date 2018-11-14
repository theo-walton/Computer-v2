#pragma once

#include "Global.hpp"
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include "Expression.hpp"
#include "MonoFunction.hpp"
#include "solve.hpp"

class	Interpreter
{
	std::map<std::string, Variable> _variables;
	std::map<std::string, MonoFunction> _functions;
	std::map<std::string, size_t> _tags;

	static std::string	normalizeWhitespace(const std::string&);

	static std::string	getVar(const std::string& line, std::string* arg);

	static bool		isQuestion(const std::string&);
	static bool		validExpr(const std::string&);
	static std::string	getExpr(const std::string&);

	std::string	expandRunit(const std::string&);
	std::string	expandPrint(const std::string&);
	std::string	expandTag(const std::string&);
	std::string	expandGoto(const std::string&);
	std::string	expandIfnt(const std::string&, bool& failed);

public:
	std::string	ReadLine(const std::string&, bool output=false);
};
