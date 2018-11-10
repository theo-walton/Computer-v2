#include "Complex.hpp"

Complex	Complex::operator*(const Complex& n) const
{
	return Complex{a * n.a - b * n.b, a * n.b + b * n.a};
}

Complex	Complex::operator/(const Complex& n) const
{
	float r = (a * n.a + b * n.b) / (n.a * n.a + n.b * n.b);
	float i = (b * n.a - a * n.b) / (n.a * n.a + n.b * n.b);
	return Complex{r, i};
}

Complex Complex::operator+(const Complex& n) const
{
	return Complex{a + n.a, b + n.b};
}

Complex Complex::operator-(const Complex& n) const
{
	return Complex{a - n.a, b - n.b};
}

Complex Complex::operator%(const Complex& n) const
{
	return Complex{std::fmod(a, n.a), std::fmod(b, n.b)};
}

Complex Complex::operator^(const Complex& n) const
{
	float f = std::pow(a * a + b * b, n.a / 2.0) * std::exp(-n.b * atan(b / a));
	float x = n.a * std::atan(b / a) + 0.5 * n.b * std::log(a * a + b * b);
	return Complex{f * std::cos(x), f * std::sin(x)};
}

std::ostream& operator<<(std::ostream& stream, const Complex& n)
{
	if (n.a)
	{
		stream << n.a;
		if (n.b > 0)
			stream << " + " << n.b << "i";
		else if (n.b < 0)
			stream << " - " << -n.b << "i";
		return stream;
	}
	if (n.b == 1)
		stream << "i";
	else if (n.b == -1)
		stream << "-i";
	else if (n.b != 0)
		stream << n.b << "i";
	else
		stream << "0";
	return stream;
}

std::istream& operator>>(std::istream& stream, Complex& n)
{
	int start = stream.tellg();
	std::string s;
	stream >> s;
	size_t bytes;
	try
	{
		if (s[0] == '+' || s[0] == '-')
			throw std::exception();
		if (s[0] == 'i')
		{
			n.a = 0;
			n.b = 1;
			stream.seekg(start + 1);
			return stream;
		}
		double d = std::stod(s, &bytes);
		if (s.length() > bytes && s[bytes] == 'i')
		{
			n.a = 0;
			n.b = d;
			stream.seekg(start + int(bytes + 1));
		}
		else
		{
			n.a = d;
			n.b = 0;
			stream.seekg(start + int(bytes));
		}
	}
	catch (std::exception)
	{
		stream.clear();
		stream.seekg(start);
		stream.setstate(std::istream::failbit);
	}	
	return stream;
}
