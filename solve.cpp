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


Complex	square_root(Complex n)
{
	Complex out;
	double a_sq = (n.a + sqrt(n.a * n.a + n.b * n.b)) / 2.0;
	out.a = sqrt(a_sq);
	out.b = sqrt(a_sq - n.a);
	return out;
}

static std::vector<Complex> solve_quad(Complex a, Complex b, Complex c)
{
	std::vector<Complex> out;
	Complex zero = {0, 0};
	Complex disc = b * b - Complex{4.0, 0} * a * c;

	if (disc.a == 0 && disc.b == 0)
	{
		out.push_back((zero - b) / (Complex{2.0, 0} * a));
		return out;
	}
	out.push_back((zero - b + square_root(disc)) / (Complex{2.0, 0} * a));
	out.push_back((zero - b - square_root(disc)) / (Complex{2.0, 0} * a));
	return out;
}

static Matrix	get_solutions(Polynomial pol)
{
	int degree = 0;
	Complex a, b, c;
	for (auto pair : pol.map)
	{
		if (pair.first == 0)
			c = pair.second;
		else if (pair.first == 1)
		{
			b = pair.second;
			degree = 1;
		}
		else if (pair.first == 2)
		{
			a = pair.second;
			degree = 2;
		}
		else
			throw std::runtime_error("cannot solve polynomial");
	}

	if (degree == 0)
		throw std::runtime_error("unknown param required to solve polynomial");
	Matrix out;
	Complex zero = {0, 0};
	if (degree == 1)
	{
		out.values.resize(1);
		out.values[0].push_back((zero - c) / b);
		return out;
	}
	out.values.resize(1);
	std::vector<Complex> roots = solve_quad(a, b, c);
	for (auto& root : roots)
		out.values[0].push_back(root);
	return out;
}

static std::string	solve(const std::string& expr, const std::map<std::string, Variable>& scope)
{
	std::vector<RPN<Variable>> rpn = Expression::create_rpn(expr);
	normalize_rpn(rpn, scope);
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
			polytok.var.map = p;
		}
		else
		{
			polytok.op = tok.op;
			polytok.type = RPN<Polynomial>::OP;
		}
		poly.push_back(polytok);
	}
	Polynomial pol = eval_rpn<Polynomial>(poly, do_ops);
	for (auto it = pol.map.begin(); it != pol.map.end();)
		if (it->second.a == 0 && it->second.b == 0)
			it = pol.map.erase(it);
		else
			++it;
	Matrix sol = get_solutions(pol);
	std::stringstream ss;
	ss << sol;
	return ss.str();
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
		std::cout << out.substr(match) << std::endl;
		if ((match == 0 || !std::isalpha(out[match - 1])) &&
		    ((match + 6 < out.length() && !std::isalpha(out[match + 6])) ||
		     match + 6 == out.length()))
		{
			std::pair<size_t, size_t> arg = get_brackets(match + 6, out);
			std::string repl = solve(out.substr(arg.first + 1, arg.second - arg.first - 1), scope);
			out.replace(match, arg.second - match + 1, repl);
			match += repl.length();
		}
		else
			match += 6;
		match = out.find("_solve", match);
	}
	return out;
}
