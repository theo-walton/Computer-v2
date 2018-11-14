#include "MonoFunction.hpp"

MonoFunction::MonoFunction(const std::string& function, const std::string& var, const std::string& arg)
{
	_var = var;
	_function = function;
	size_t match = _function.find(arg);
	while (match != std::string::npos)
	{
		if ((match == 0 || !std::isalpha(_function[match - 1])) &&
		    ((match + arg.length() < _function.length() && !std::isalpha(_function[match + arg.length()])) ||
		     match + arg.length() == _function.length()))
		{
			_function.replace(match, arg.length(), "$");
		}
		match += arg.length();
		match = _function.find(arg, match);
	}
}

std::string	MonoFunction::_replace(const std::string& arg)
{
	std::string out = _function;
	std::string replacement;
	replacement = replacement + "(" + arg + ")";
	size_t pos = out.find("$");
	while (pos != std::string::npos)
	{
		out.replace(pos, 1, replacement);
		pos = out.find("$");
	}
	return out;
}

static std::pair<size_t, size_t> get_brackets(size_t begin, const std::string& str)
{
	std::pair<size_t, size_t> out;
	out.first = begin;
	if (str[begin] != '(')
		throw std::runtime_error("expected open bracket after function name");
	int bracket = 1;
	size_t pos = begin;
	while (pos < str.size() && bracket != 0)
	{
		pos++;
		if (str[pos] == ')')
			bracket--;
		else if (str[pos] == '(')
			bracket++;
	}
	if (bracket != 0)
		throw std::runtime_error("bad brackets on function");
	out.second = pos;
	return out;
}

std::string	MonoFunction::Expand(const std::string& expression)
{
	std::string expr = expression;
	size_t match = expr.find(_var);
	while (match != std::string::npos)
	{
		if ((match == 0 || !std::isalpha(expr[match - 1])) &&
		    ((match + _var.length() < expr.length() && !std::isalpha(expr[match + _var.length()])) ||
		     match + _var.length() == expr.length()))
		{
			std::pair<size_t, size_t> arg = get_brackets(match + _var.length(), expr);
			std::string replacement = _replace(expr.substr(arg.first + 1, arg.second - arg.first - 1));
			expr.replace(match, arg.second - match + 1, replacement);
			match += arg.first - arg.second - 1 + replacement.length();
		}
		match += _var.length();
		match = expr.find(_var, match);
	}
	return expr;
}
