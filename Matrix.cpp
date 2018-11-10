#include "Matrix.hpp"
#include "Expression.hpp"

bool	Matrix::IntegrityCheck() const
{
	size_t prev;
	if (values.empty())
		return false;
	for (size_t i = 0; i < values.size(); i++)
	{
		if (values[i].empty())
			return false;
		if (i && values[i].size() != prev)
			return false;
		prev = values[i].size();
	}
	return true;
}

Matrix Matrix::operator*(const Matrix& m) const
{
	assert(IntegrityCheck());
	assert(m.IntegrityCheck());
	if (!values.size() ||
	    values.size() != m.values.size() ||
	    !values[0].size() ||
	    values[0].size() != m.values[0].size())
	{
		throw std::runtime_error("incompatible matrix operation");
	}

	Matrix out = m;
	for (size_t i = 0; i < values.size(); i++)
		for (size_t j = 0; j < values[i].size(); j++)
			out.values[i][j] = values[i][j] * m.values[i][j];
	return out;
}

Matrix Matrix::operator/(const Matrix& m) const
{
	assert(IntegrityCheck());
	assert(m.IntegrityCheck());
	if (!values.size() ||
	    values.size() != m.values.size() ||
	    !values[0].size() ||
	    values[0].size() != m.values[0].size())
	{
		throw std::runtime_error("incompatible matrix operation");
	}

	Matrix out = m;
	for (size_t i = 0; i < values.size(); i++)
		for (size_t j = 0; j < values[i].size(); j++)
			out.values[i][j] = values[i][j] / m.values[i][j];
	return out;
}

Matrix Matrix::operator+(const Matrix& m) const
{
	assert(IntegrityCheck());
	assert(m.IntegrityCheck());
	if (!values.size() ||
	    values.size() != m.values.size() ||
	    !values[0].size() ||
	    values[0].size() != m.values[0].size())
	{
		throw std::runtime_error("incompatible matrix operation");
	}

	Matrix out = m;
	for (size_t i = 0; i < values.size(); i++)
		for (size_t j = 0; j < values[i].size(); j++)
			out.values[i][j] = values[i][j] + m.values[i][j];
	return out;
}

Matrix Matrix::operator-(const Matrix& m) const
{
throw std::runtime_error("incompatible matrix operation");	assert(IntegrityCheck());
	assert(m.IntegrityCheck());
	if (!values.size() ||
	    values.size() != m.values.size() ||
	    !values[0].size() ||
	    values[0].size() != m.values[0].size())
	{
		throw std::runtime_error("incompatible matrix operation");
	}

	Matrix out = m;
	for (size_t i = 0; i < values.size(); i++)
		for (size_t j = 0; j < values[i].size(); j++)
			out.values[i][j] = values[i][j] - m.values[i][j];
	return out;
}

Matrix Matrix::operator%(const Matrix& m) const
{
	assert(IntegrityCheck());
	assert(m.IntegrityCheck());
	if (!values.size() ||
	    values.size() != m.values.size() ||
	    !values[0].size() ||
	    values[0].size() != m.values[0].size())
	{
		throw std::runtime_error("incompatible matrix operation");
	}

	Matrix out = m;
	for (size_t i = 0; i < values.size(); i++)
		for (size_t j = 0; j < values[i].size(); j++)
			out.values[i][j] = values[i][j] % m.values[i][j];
	return out;
}

Matrix Matrix::operator^(const Matrix& m) const
{
	assert(IntegrityCheck());
	assert(m.IntegrityCheck());
	if (!values.size() ||
	    values.size() != m.values.size() ||
	    !values[0].size() ||
	    values[0].size() != m.values[0].size())
	{
		throw std::runtime_error("incompatible matrix operation");
	}

	Matrix out = m;
	for (size_t i = 0; i < values.size(); i++)
		for (size_t j = 0; j < values[i].size(); j++)
			out.values[i][j] = values[i][j] ^ m.values[i][j];
	return out;
}

Matrix	Matrix::Multiply(const Matrix& a, const Matrix& b)
{
	assert(a.IntegrityCheck());
	assert(b.IntegrityCheck());
	if (!a.values.size() || !b.values.size() ||
	    b.values.size() != a.values[0].size())
	{
		throw std::runtime_error("incompatible matrix operation");
	}

	Matrix out;
	out.values.resize(a.values.size());
	for (auto& row : out.values)
	{
		row.resize(b.values[0].size());
		for (auto& var : row)
			var = Complex{0, 0};
	}

	for (size_t ax = 0; ax < a.values.size(); ax++)
		for (size_t ay = 0; ay < a.values[0].size(); ay++)
			for (size_t bx = 0; bx < b.values.size(); bx++)
				for (size_t by = 0; by < b.values[0].size(); by++)
					out.values[ax][by] = out.values[ax][by] + a.values[ax][ay] * b.values[bx][by];
	return out;
}

std::ostream& operator<<(std::ostream& stream, const Matrix& m)
{
	for (int i = 0; i < m.values.size(); i++)
	{
		bool first = true;
		stream << "[";
		for (int j = 0; j < m.values[0].size(); j++)
		{
			if (!first)
				stream << ",";
			stream << " " << m.values[i][j] << " ";
			first = false;
		}
		stream << "]";
		if (i != m.values.size() - 1)
			stream << "\n";
	}
	return stream;
}

static std::string	get_raw_matrix(std::istream& stream)
{
	while (std::isspace(stream.peek()))
		stream.ignore();
	if (stream.peek() != '[')
		return "";
	// get matching closing brace
	int begin = stream.tellg();
	stream.ignore();
	int bracket = 1;
	int c;

	while (1)
	{
		c = stream.get();
		if (c == '[')
			bracket++;
		else if (c == ']')
			bracket--;
		if (bracket == 0)
			break;
		if (c == EOF)
			return "";
	}
	int end = stream.tellg();
	int matrix_size = end - begin - 1;
	char* raw_matrix = new char[matrix_size];
	stream.seekg(begin + 1);
	stream.read(raw_matrix, matrix_size);
	raw_matrix[matrix_size - 1] = '\0';
	std::string out(raw_matrix);
	delete[] raw_matrix;
	return out;
}

static bool	parse_complex(const std::string& token, Complex* c)
{
	try
	{
		Variable v = Expression::evaluate(token, std::map<std::string, Variable>());
		if (v.type == Variable::COMPLEX)
		{
			*c = v.complex;
			return true;
		}
	}
	catch (std::exception){}
	return false;
}

static std::vector<Complex>	parse_row(const std::string& row)
{
	std::cout << "Row: " << row << std::endl;
	std::stringstream ss(row + ",");
	std::vector<Complex> out;

	for (std::string token; std::getline(ss, token, ',');)
	{
		Complex c;
		if (!parse_complex(token, &c))
			return std::vector<Complex>();
		out.push_back(c);
	}
	return out;
}

static bool	fill_matrix(std::string& str, Matrix& m)
{
	std::stringstream ss(str + ";");
	Matrix temp;

	for (std::string token; std::getline(ss, token, ';');)
		if (token[0] == '[' && token[token.length() - 1] == ']')
			temp.values.push_back(parse_row(token.substr(1, token.length() - 2)));
		else
			return false;

	if (!temp.IntegrityCheck())
		return false;
	m = temp;
	return true;
}

std::istream& operator>>(std::istream& stream, Matrix& m)
{
	int start = stream.tellg();
	std::string str = get_raw_matrix(stream);
	str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
	std::cout << "raw matrix: " << str << std::endl;
	if (str.empty() || !fill_matrix(str, m))
	{
		stream.clear();
		stream.seekg(start);
		stream.setstate(std::istream::failbit);
	}
	return stream;
}
