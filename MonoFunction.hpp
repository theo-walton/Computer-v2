#pragma once

#include <string>
#include <utility>
#include <iostream>

// very similar to a single argument macro... uninteligently expands argument
class	MonoFunction
{
	std::string _function;
	std::string _var;

	std::string	_replace(const std::string& arg);
public:
	MonoFunction(const std::string& function, const std::string& var, const std::string& arg);
	std::string	Expand(const std::string& expression);
};
