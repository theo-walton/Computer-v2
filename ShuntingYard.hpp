#pragma once

#include "ExceptionTypes.hpp"
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <functional>
#include <iostream>

template<class T>
struct	RPN
{
	std::string op;
	T var;
	enum Type
	{
		OP,
		VAR
	};
	Type type;
};

	

template<class T>
RPN<T>
get_token(std::string& expr,
	  const std::vector<std::string>& ops,
	  const std::function<T(const std::string&, size_t*)>& f)
{
	try
	{
		RPN<T> out;
		size_t bytes;
		out.var = f(expr, &bytes);
		expr = expr.substr(bytes);
		out.type = RPN<T>::VAR;
		return out;
	}
	catch (std::exception){}

	for (auto op : ops)
	{
		if (expr.compare(0, op.length(), op) == 0)
		{
			RPN<T> out;
			out.type = RPN<T>::OP;
			out.op = op;
			expr = expr.substr(op.length());
			return out;
		}
	}
	throw std::runtime_error("invalid token in expression");
}

template<class T>
std::vector<RPN<T>>
shunting_yard(std::string expr,
	      const std::vector<std::string>& ops,
	      const std::map<std::string, int>& priority,
	      const std::function<T(const std::string&, size_t*)>& f,
	      const std::string& open = "(",
	      const std::string& close = ")")
{
	std::vector<RPN<T>> rpn;
	std::stack<RPN<T>> stack;
	bool end = false;
	auto prio = [&](const std::string& op){
			    auto it = priority.find(op);
			    return it == priority.end() ? -1 : it->second;
		    };
	while (1)
	{
		if (end)
		{
			while (stack.size() != 0)
			{
				if (stack.top().op == open)
					throw std::runtime_error("open bracket remaining in stack");
				rpn.push_back(stack.top());
				stack.pop();
			}
			break;
		}
		RPN<T> tok = get_token(expr, ops, f);
		if (expr.empty())
			end = true;
		if (tok.type == RPN<T>::VAR)
			rpn.push_back(tok);
		else if (tok.op == open)
			stack.push(tok);
		else if (tok.op == close)
		{
			if (stack.size() == 0)
				throw std::runtime_error("closing bracket lacks pair");
			while (stack.top().op != open)
			{
				rpn.push_back(stack.top());
				stack.pop();
				if (stack.size() == 0)
					throw std::runtime_error("closing bracket lacks pair");
			}
			if (stack.size() != 0 && stack.top().op == open)
				stack.pop();
		}
		else
		{
			while (stack.size() != 0 && prio(tok.op) <= prio(stack.top().op))
			{
				rpn.push_back(stack.top());
				stack.pop();
			}
			stack.push(tok);
		}
	}
	return rpn;
}

template<class T>
T
eval_rpn(std::vector<RPN<T>> rpn,
	 const std::function<void(std::stack<T>&, const std::string&)>& f)
{
	std::stack<T> stack;
	for (const auto& tok : rpn)
	{
		if (tok.type == RPN<T>::OP)
			f(stack, tok.op);
		else
			stack.push(tok.var);
	}
	if (stack.size() != 1)
		throw std::runtime_error("too many operands remaining on stack");
	return stack.top();
}
