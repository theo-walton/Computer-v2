#include "Polynomial.hpp"

Polynomial Polynomial::operator*(const Polynomial& p) const
{
	Polynomial out;
	for (auto& p1 : map)
		for (auto& p2 : p.map)
			out.map[p1.first + p2.first] = out.map[p1.first + p2.first] + p1.second * p2.second;
	return out;
}

Polynomial Polynomial::operator/(const Polynomial& p) const
{
	Polynomial out;
	if (p.map.size() != 1)
		throw std::runtime_error("invalid polynomial resulting");
	auto monomial = *p.map.begin();
	for (auto x : map)
		out.map[x.first - monomial.first] = x.second / monomial.second;
	return out;
}

Polynomial Polynomial::operator%(const Polynomial& p) const
{
	if (p.map.size() != 1 || p.map.count(0) != 1 ||
	    map.size() != 1 || map.count(0) != 1)
	{
		throw std::runtime_error("invalid polynomial resulting");
	}
	Polynomial out;
	out.map[0] = map.at(0) * p.map.at(0);
	return out;
}

Polynomial Polynomial::operator+(const Polynomial& p) const
{
	Polynomial out = {map};
	for (auto& pair : p.map)
		out.map[pair.first] = out.map[pair.first] + pair.second;
	return out;
}

Polynomial Polynomial::operator-(const Polynomial& p) const
{
	Polynomial out = {map};
	for (auto& pair : p.map)
		out.map[pair.first] = out.map[pair.first] - pair.second;
	return out;
}

Polynomial Polynomial::operator^(const Polynomial& p) const
{
	Polynomial out;
	if (p.map.size() == 1 && p.map.count(0) == 1)
	{
		Complex c = p.map.at(0);
		if (c.b)
			throw std::runtime_error("invalid polynomial resulting");
		int pow = roundf(c.a);
		if (abs(double(pow) - c.a) > 0.001)
			throw std::runtime_error("invalid polynomial resulting");
		for (auto& pair : map)
			out.map[pair.first * pow] = pair.second ^ Complex{static_cast<float>(pow), 0};
	}
	else
		throw std::runtime_error("invalid polynomial resulting");
	return out;
}

std::ostream& operator<<(std::ostream& stream, const Polynomial& p)
{
	bool first = true;
	for (auto pair : p.map)
	{
		if (!first)
			stream << ",";
		stream << "(" << pair.second << ")$^" << pair.first;
		first = false;
	}
	return stream;
}
