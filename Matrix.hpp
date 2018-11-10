#pragma once

#include "ExceptionTypes.hpp"
#include "Complex.hpp"
#include <vector>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <map>

struct	Matrix
{
	// _values[0][3] -> row 0, column 3
	std::vector<std::vector<Complex>> values;

	bool	IntegrityCheck() const;

	// term to term ops
	Matrix operator*(const Matrix&) const;
	Matrix operator/(const Matrix&) const;
	Matrix operator+(const Matrix&) const;
	Matrix operator-(const Matrix&) const;
	Matrix operator%(const Matrix&) const;
	Matrix operator^(const Matrix&) const;

	static Matrix	Multiply(const Matrix&, const Matrix&);
};

std::ostream& operator<<(std::ostream&, const Matrix&);
std::istream& operator>>(std::istream&, Matrix&);
