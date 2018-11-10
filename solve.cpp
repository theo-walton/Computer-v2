#include "solve.hpp"

static void	normalize_rpn(std::vector<RPN<Variable>>& rpn, const std::map<std::string, Variable>& scope)
{
	std::string unknown;
	for (auto& tok : rpn)
	{
		if (tok.type == RPN<Variable>::VAR && tok.var.type == Variable::UNKNOWN)
		{
			try
			{
				tok.var	= scope.at(tok.var.unknown);
			}
			catch (std::out_of_range)
			{
				if (!unknown.empty() && unknown != tok.var.unknown)
					throw std::runtime_error("unknown variable encountered");
				unknown = tok.var.unknown;
				tok.var.unknown	= "$";
			}
		}
	}
}

static void
do_ops(std::stack<Polynomial>& stack,
       const std::string& op)
{
	if (op == "~" || op == "#")
	{
		if (stack.size() < 1)
			throw std::runtime_error("not enough operands in stack");
		Polynomial a = stack.top();
		stack.pop();

		if (op == "#")
			stack.push(a);
		else if (op == "~")
		{
			Polynomial zero;
			zero.map[0] = Complex{0, 0};
			stack.push(zero - a);
		}
		return;
	}
	if (stack.size() < 2)
		throw std::runtime_error("not enough operands in stack");
	Polynomial a = stack.top();
	stack.pop();
	Polynomial b = stack.top();
	stack.pop();

	if (op == "^")
		stack.push(b ^ a);
	else if (op == "**")
		stack.push(Variable::MatrixMult(b, a));
	else if (op == "*")
		stack.push(b * a);
	else if (op == "/")
		stack.push(b / a);
	else if (op == "%")
		stack.push(b % a);
	else if (op == "+")
		stack.push(b + a);
	else if (op == "-")
		stack.push(b - a);
	else
		throw std::runtime_error("unknown operator in stack");
}

static Polynomial	eval_polynomial(std::vector<RPN<Polynomial>>& poly)
{
	auto do_operation = [](){};
	eval_rpn<Polynomial>(poly, do_operation);
}

static std::string	solve(const std::string& expr, const std::map<std::string, Variable>& scope)
{
	std::cout << "attempting to solve: '" << expr << "'" << std::endl;

	std::vector<RPN<Variable>> rpn = Expression::create_rpn(expr);
	normalize_rpn(rpn, scope);

	for (auto tok : rpn)
	{
		if (tok.type == RPN<Variable>::VAR)
			std::cout << "[" << tok.var << "]";
		else
			std::cout << "[" << tok.op << "]";
	}
	std::cout << std::endl;

	std::vector<RPN<Polynomial>> poly;

	for (auto tok : rpn)
	{
		RPN<Polynomial> polytok;
		if (tok.type == RPN<Variable>::VAR)
		{
			std::map<int, Complex> p;
			if (tok.var.unknown == "$")
				p[1] = Complex{1.0, 0};
			else if (tok.var.type == Variable::COMPLEX)
				p[0] = tok.var.complex;
			else
				throw std::runtime_error("_solve() does not work on matrix types");
			polytok.type = RPN<Polynomial>::VAR;
		}
		else
		{
			polytok.op = tok.op;
			polytok.type = RPN<Polynomial>::OP;
		}
	}
	Polynomial pol = eval_polynomial(poly)
	
	return "[[0]]";
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

std::string	expand_solve(const std::string& expr, const std::map<std::string, Variable>& scope)
{
	std::string out = expr;
	size_t match = out.find("_solve");
	while (match != std::string::npos)
	{
		if ((match == 0 || !std::isalpha(out[match - 1])) &&
		    ((match + 6 < out.length() && !std::isalpha(out[match + 6])) ||
		     match + 6 == out.length()))
		{
			std::pair<size_t, size_t> arg = get_brackets(match + 6, out);
			std::string repl = solve(out.substr(arg.first + 1, arg.second - arg.first - 1), scope);
			std::cout << "replacement: " << repl << std::endl;
			out.replace(match, arg.second - match + 1, repl);
			match += arg.first - arg.second - 1 + repl.length();
		}
		match += 6;
		match = out.find("_solve", match);
	}
	return out;
}
