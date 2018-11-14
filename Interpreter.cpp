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

std::string	Interpreter::expandRunit(const std::string& expression)
{
	std::string out = expression;
	size_t match = out.find("_runit");
	while (match != std::string::npos)
	{
		if ((match == 0 || !std::isalpha(out[match - 1])) &&
		    ((match + 6 < out.length() && !std::isalpha(out[match + 6])) ||
		     match + 6 == out.length()))
		{
			std::pair<size_t, size_t> arg = get_brackets(match + 6, out);
			std::string repl = ReadLine(out.substr(arg.first + 1, arg.second - arg.first - 1), true);
			out.replace(match, arg.second - match + 1, repl);
			match += repl.length();
		}
		else
			match += 6;
		match = out.find("_runit", match);
	}
	return out;
}

std::string	Interpreter::expandPrint(const std::string& expression)
{
	std::string out = expression;
	size_t match = out.find("_print");
	while (match != std::string::npos)
	{
		if ((match == 0 || !std::isalpha(out[match - 1])) &&
		    ((match + 6 < out.length() && !std::isalpha(out[match + 6])) ||
		     match + 6 == out.length()))
		{
			std::pair<size_t, size_t> arg = get_brackets(match + 6, out);
			std::string print = ReadLine(out.substr(arg.first + 1, arg.second - arg.first - 1), true);
			std::cout << print << std::endl;
			std::string repl = "";
			out.replace(match, arg.second - match + 1, repl);
			match += repl.length();
		}
		else
			match += 6;
		match = out.find("_print", match);
	}
	return out;
}

std::string	Interpreter::expandTag(const std::string& expression)
{
	std::string out = expression;
	size_t match = out.find("_tag");
	while (match != std::string::npos)
	{
		if ((match == 0 || !std::isalpha(out[match - 1])) &&
		    ((match + 4 < out.length() && !std::isalpha(out[match + 4])) ||
		     match + 4 == out.length()))
		{
			std::pair<size_t, size_t> arg = get_brackets(match + 4, out);
			std::string tag = ReadLine(out.substr(arg.first + 1, arg.second - arg.first - 1), true);
			_tags[tag] = Global::line_num - 1;
			std::string repl = "";
			out.replace(match, arg.second - match + 1, repl);
			match += repl.length();
		}
		else
			match += 4;
		match = out.find("_tag", match);
	}
	return out;
}

std::string	Interpreter::expandGoto(const std::string& expression)
{
	std::string out = expression;
	size_t match = out.find("_goto");
	while (match != std::string::npos)
	{
		if ((match == 0 || !std::isalpha(out[match - 1])) &&
		    ((match + 5 < out.length() && !std::isalpha(out[match + 5])) ||
		     match + 5 == out.length()))
		{
			std::pair<size_t, size_t> arg = get_brackets(match + 5, out);
			std::string o = ReadLine(out.substr(arg.first + 1, arg.second - arg.first - 1), true);
			if (_tags.count(o) != 0)
				Global::line_num = _tags[o];
			std::string repl = "";
			out.replace(match, arg.second - match + 1, repl);
			match += repl.length();
		}
		else
			match += 5;
		match = out.find("_goto", match);
	}
	return out;
}

std::string	Interpreter::ReadLine(const std::string& line, bool output)
{
	std::string lin = expandTag(line);
	lin = expandPrint(lin);
	lin = expandRunit(lin);
	lin = expandGoto(lin);
	std::string temp = lin;
	temp.erase(remove_if(temp.begin(), temp.end(), ::isspace), temp.end());
	if (temp.empty())
		return "";
	std::string arg;
	std::string var = getVar(lin, &arg);
	std::string expr = getExpr(lin);

	// up to 3 layers of nesting is well defined for functions... any more is UB lol
	for (auto& f : _functions)
			expr = f.second.Expand(expr);
	for (auto& f : _functions)
                        expr = f.second.Expand(expr);
	for (auto& f : _functions)
                        expr = f.second.Expand(expr);
	if (arg.empty())
	{
		std::cout << "before: " << expr << std::endl;
		expr = expand_solve(expr, _variables);
		std::cout << "after: " << expr << std::endl;
		_variables[var] = Expression::evaluate(expr, _variables);
		if (output)
		{
			std::stringstream ss;
			ss << _variables[var];
			return ss.str();
		}
		else
			return "";
	}
	MonoFunction fun(expr, var, arg);
	_functions.erase(var);
	_functions.insert({var, fun});
	if (output)
	{
		std::stringstream ss;
		ss << expr;
		return ss.str();
	}
	return "";
}
