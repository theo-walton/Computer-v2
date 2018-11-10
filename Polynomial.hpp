#pragma once

#include <map>
#include "Complex.hpp"

struct	Polynomial
{
	std::map<int, Complex> map;
	
	Polynomial operator*(const Polynomial&) const;
	Polynomial operator/(const Polynomial&) const;
	Polynomial operator+(const Polynomial&) const;
	Polynomial operator-(const Polynomial&) const;
	Polynomial operator^(const Polynomial&) const;
};
