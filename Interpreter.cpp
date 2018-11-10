#include "Interpreter.hpp"

std::string	Interpreter::normalizeWhitespace(const std::string& s)
{
	std::string out;
	bool previous_space = true;
	for (auto c : s)
	{
		if (std::isspace(c))
		{
			if (!previous_space)
			{
				out.push_back(' ');
				previous_space = true;
			}
		}
		else
		{
			out.push_back(c);
			previous_space = false;
		}
	}
	if (out.length() && out[out.length() - 1] == ' ')
		out.pop_back();
	return out;
}

std::string	Interpreter::getVar(const std::string& line, std::string* arg)
{
	size_t index = line.find_first_of("=");
	if (index == std::string::npos)
		return "NONE";
	if (isQuestion(line.substr(index + 1)))
		return "NONE";

	std::string var = normalizeWhitespace(line.substr(0, index));
	std::transform(var.begin(), var.end(), var.begin(), ::tolower);

	size_t open_b = var.find_first_of("(");
	size_t close_b = var.find_last_of(")");
	if (open_b != std::string::npos &&
	    close_b != std::string::npos &&
	    close_b > open_b)
	{
		*arg = var.substr(open_b + 1, close_b - open_b - 1);
	}
	if (close_b != std::string::npos && !var.substr(close_b + 1).empty())
		throw std::runtime_error("invalid variable expression");
	if (open_b != std::string::npos)
		var = var.substr(0, open_b);
	if (!Variable::validVarName(var) || (!arg->empty() && !Variable::validVarName(*arg)))
		throw std::runtime_error("invalid variable name");
	return var;
}

bool	Interpreter::isQuestion(const std::string& question)
{
	for (auto c : question)
		if (c != '?' && c != ' ')
			return false;
	return true;
}

bool	Interpreter::validExpr(const std::string& expr)
{
	std::string allowed_ops = "+-()/*%^ ;[],_";
	for (auto c : expr)
	{
		bool allowed = false;
		for (auto op : allowed_ops)
			if (c == op)
				allowed = true;
		if (c >= 'a' && c <= 'z')
			allowed = true;
		if (c >= '0' && c <= '9')
			allowed = true;
		if (!allowed)
			return false;
	}
	return true;
}

std::string	Interpreter::getExpr(const std::string& line)
{
	size_t index = line.find_first_of("=");
	std::string expr;
	if (index != std::string::npos)
	{
		expr = line.substr(index + 1);
		if (isQuestion(expr))
			expr = line.substr(0, index);
	}
	else
		expr = line;
	expr = normalizeWhitespace(expr);
	std::transform(expr.begin(), expr.end(), expr.begin(), ::tolower);

	if (!validExpr(expr))
		throw std::runtime_error("invalid characters in expression");
	return expr;
}



void	Interpreter::ReadLine(const std::string& line)
{
	std::string arg;
	std::string var = getVar(line, &arg);
	std::string expr = getExpr(line);

	std::cout << "arg: " << arg << std::endl;
	std::cout << "var: " << var << std::endl;
	std::cout << "expr: " << expr << std::endl;

	// up to 3 layers of nesting is well defined for functions... any more is UB lol
	for (auto& f : _functions)
			expr = f.second.Expand(expr);
	for (auto& f : _functions)
                        expr = f.second.Expand(expr);
	for (auto& f : _functions)
                        expr = f.second.Expand(expr);
	expr = expand_solve(expr, _variables);
	if (arg.empty())
	{
		_variables[var] = Expression::evaluate(expr, _variables);
		std::cout << _variables[var] << std::endl;
		return;
	}
	MonoFunction fun(expr, var, arg);
	_functions.insert({var, fun});
	std::cout << expr << std::endl;
}
