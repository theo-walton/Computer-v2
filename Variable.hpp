#pragma once

#include "ExceptionTypes.hpp"
#include "Complex.hpp"
#include "Matrix.hpp"

struct	Variable
{
	Complex complex;
	Matrix matrix;
	std::string unknown;
	enum Type
	{
		COMPLEX,
		MATRIX,
		UNKNOWN
	};
	Type type;

	Variable();
	Variable operator*(const Variable&);
	Variable operator/(const Variable&);
	Variable operator+(const Variable&);
	Variable operator-(const Variable&);
	Variable operator%(const Variable&);
	Variable operator^(const Variable&);

	static Variable MatrixMult(const Variable&, const Variable&);

	static bool	validVarName(const std::string&);
};

std::ostream& operator<<(std::ostream&, const Variable&);
std::istream& operator>>(std::istream&, Variable&);

