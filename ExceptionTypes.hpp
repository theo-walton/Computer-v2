#pragma once

#include <exception>
#include <string>
#include <iostream>

class	Error : public std::exception
{
	std::string error;
public:
	Error() : error("") {}
	Error(std::string e) : error(e) {}
	const char*	what() const noexcept
	{
		std::cout << error << std::endl;
		return error.c_str();
	}
};
