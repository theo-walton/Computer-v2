#pragma once

#include <iostream>
#include <cmath>
#include <sstream>

struct	Complex
{
	float a = 0;
	float b = 0;

	Complex operator*(const Complex&) const;
	Complex operator/(const Complex&) const;
	Complex operator+(const Complex&) const;
	Complex operator-(const Complex&) const;
	Complex operator%(const Complex&) const;
	Complex operator^(const Complex&) const;
};

std::ostream& operator<<(std::ostream&, const Complex&);
std::istream& operator>>(std::istream&, Complex&);
