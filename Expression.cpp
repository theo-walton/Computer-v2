#include "Expression.hpp"

static void
do_ops(std::stack<Variable>& stack,
       const std::string& op,
       const std::map<std::string, Variable>& scope)
{
	if (op == "~" || op == "#")
	{
		if (stack.size() < 1)
			throw std::runtime_error("not enough operands in stack");
		Variable a = stack.top();
		stack.pop();

		if (a.type == Variable::UNKNOWN)
		{
			try
			{
				a = scope.at(a.unknown);
			}
			catch (std::out_of_range)
			{
				throw std::runtime_error("unknown variable encountered");
			}
		}

		if (op == "#")
			stack.push(a);
		else if (op == "~")
		{
			Variable zero;
			zero.type = Variable::COMPLEX;
			zero.complex = {0, 0};
			stack.push(zero - a);
		}
		return;
	}
	if (stack.size() < 2)
		throw std::runtime_error("not enough operands in stack");
	Variable a = stack.top();
	stack.pop();
	Variable b = stack.top();
	stack.pop();

	if (a.type == Variable::UNKNOWN)
	{
		try
		{
			a = scope.at(a.unknown);
		}
		catch (std::out_of_range)
		{
			throw std::runtime_error("unknown variable encountered");
		}
	}
	if (b.type == Variable::UNKNOWN)
	{
		try
		{
			b = scope.at(b.unknown);
		}
		catch (std::out_of_range)
		{
			throw std::runtime_error("unknown variable encountered");
		}
	}

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

static Variable	var_parse(const std::string& str, size_t* bytes)
{
	std::stringstream ss(str);
	Variable out;

	int start = ss.tellg();
	ss >> out;
	if (ss.fail())
	{
		throw std::runtime_error("invalid operand");
	}
	*bytes = size_t(int(ss.tellg()) - start);
	return out;
}

static std::string	normalize_expr(const std::string& expr)
{
	std::string str = expr;
	str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
	for (size_t i = 0; i < str.size(); i++)
	{
		if (i == 0 || std::string("^*/%(+-#~").find(str[i - 1]) != std::string::npos)
		{
			if (i != 0 && (str[i] == '+' || str[i] == '-') &&
			    (str[i - 1] == '#' || str[i - 1] == '~'))
			{
				throw std::runtime_error("cannot have 2 unary operators in row");
			}
			if (str[i] == '+')
				str[i] = '#';
			else if (str[i] == '-')
				str[i] = '~';
		}
	}
	return str;
}

Variable	Expression::evaluate(const std::string& expr, const std::map<std::string, Variable>& scope)
{
	return evaluate_rpn(create_rpn(expr), scope);
}

std::vector<RPN<Variable>>	Expression::create_rpn(const std::string& expr)
{
	std::string processed_expr = normalize_expr(expr);

	std::vector<std::string> ops = {"#", "~", "^", "**", "*", "/", "%", "+", "-", "(", ")"};
	std::map<std::string, int> priority;
	priority["#"] = 11;
	priority["~"] = 10;
	priority["^"] = 9;
	priority["**"] = 8;
	priority["*"] = 7;
	priority["/"] = 6;
	priority["%"] = 5;
	priority["+"] = 4;
	priority["-"] = 3;

	return shunting_yard<Variable>(processed_expr, ops, priority, var_parse);
}

Variable	Expression::evaluate_rpn(const std::vector<RPN<Variable>>& rpn,
					 const std::map<std::string, Variable>& scope)
{
	auto do_operation = [&scope](std::stack<Variable>& stack, const std::string& op)
				    {do_ops(stack, op, scope);};

	Variable out = eval_rpn<Variable>(rpn, do_operation);
	if (out.type == Variable::UNKNOWN)
	{
		try
		{
			out = scope.at(out.unknown);
		}
		catch (std::out_of_range)
		{
			throw std::runtime_error("unknown variable encountered");
		}
	}
	return out;
}
