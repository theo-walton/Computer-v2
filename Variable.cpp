#include "Variable.hpp"

#define OP_TEMPLATE(_OP_) \
{\
	Variable out;\
	if (type == COMPLEX && v.type == COMPLEX)\
	{\
		out.complex = complex _OP_ v.complex;\
		out.type = COMPLEX;\
	}\
	else if (type == COMPLEX && v.type == MATRIX)\
	{\
		out.matrix = v.matrix;\
		for (auto& row : out.matrix.values)\
			for (auto& var : row)\
				var = complex _OP_ var;\
		out.type = MATRIX;\
	}\
	else if (type == MATRIX && v.type == COMPLEX)\
	{\
		out.matrix = matrix;\
		for (auto& row : out.matrix.values)\
			for (auto& var : row)\
				var = var _OP_ v.complex;\
		out.type = MATRIX;\
	}\
	else if (type == MATRIX && v.type == MATRIX)\
	{\
		out.matrix = matrix _OP_ v.matrix;\
		out.type = MATRIX;\
	}\
	else\
		throw std::runtime_error("cannot do operation on variable with unknown type");\
	return out;\
}

Variable::Variable()
{
	type = UNKNOWN;
}

Variable Variable::operator*(const Variable& v)
{
	OP_TEMPLATE(*)
}

Variable Variable::operator/(const Variable& v)
{
	OP_TEMPLATE(/)
}

Variable Variable::operator+(const Variable& v)
{
	OP_TEMPLATE(+)
}

Variable Variable::operator-(const Variable& v)
{
	OP_TEMPLATE(-)
}

Variable Variable::operator%(const Variable& v)
{
	OP_TEMPLATE(%)
}

Variable Variable::operator^(const Variable& v)
{
	OP_TEMPLATE(^)
}

Variable Variable::MatrixMult(const Variable& a, const Variable& b)
{
	Variable out;

	if (a.type == MATRIX && b.type == MATRIX)
	{
		out.matrix = Matrix::Multiply(a.matrix, b.matrix);
		out.type = MATRIX;
		return out;
	}
	throw std::runtime_error("'MatrixMult' cannot work on non matrix");
}

std::ostream& operator<<(std::ostream& stream, const Variable& v)
{
	if (v.type == Variable::UNKNOWN)
		stream << v.unknown;
	else if (v.type == Variable::MATRIX)
		stream << v.matrix;
	else if (v.type == Variable::COMPLEX)
		stream << v.complex;
	return stream;
}

std::istream& operator>>(std::istream& stream, Variable& v)
{
	int begin = stream.tellg();
	while (std::isspace(stream.peek()))
		stream.ignore();
	int pos = stream.tellg();
	stream >> v.complex;
	v.type = Variable::COMPLEX;
	if (stream.fail())
	{
		stream.clear();
		stream.seekg(pos);
		stream >> v.matrix;
		v.type = Variable::MATRIX;
		if (stream.fail())
		{
			stream.clear();
			stream.seekg(pos);
			while (std::isalpha(stream.peek()))
			{
				v.unknown += stream.peek();
				stream.ignore();
			}
			v.type = Variable::UNKNOWN;
			stream.seekg(pos + int(v.unknown.length()));
			if (stream.fail() || !Variable::validVarName(v.unknown))
			{
				stream.clear();
				stream.seekg(begin);
				stream.setstate(std::istream::failbit);
			}
		}
	}
	return stream;
}

bool	Variable::validVarName(const std::string& var)
{
	for (auto c : var)
		if (c < 'a' || c > 'z')
			return false;
	if (var == "i" || var == "")
		return false;
	return true;
}
