
#include "Interpreter.hpp"
#include "Global.hpp"
#include <iostream>
#include <string>
#include <fstream>

int	main(int ac, char** av)
{
	std::istream *stream;
	std::ifstream fileStream;
	
	if (ac == 1)
	{
		stream = &std::cin;
		Global::read_from_file = false;
	}
	else if (ac == 2)
	{
		fileStream.open(av[1]);
		if (fileStream.fail() || !fileStream.good())
		{
			std::cerr << av[0] << ": " << av[1] << ": " << strerror(errno) << std::endl;
			return 1;
		}
		stream = &fileStream;
		Global::read_from_file = true;
	}
	else
	{
		std::cout << "usage: " << av[0] << " [filepath]" << std::endl;
		return 0;
	}

	std::vector<std::string> lines;
	Interpreter interpret;
	if (!Global::read_from_file)
	{
		std::cout << ">>> ";
	        std::cout.flush();
	}
	while (1)
	{
		std::string line;
		if (Global::line_num < lines.size())
			line = lines[Global::line_num];
		else
		{
			if (!std::getline(*stream, line))
				break;
			lines.push_back(line);
		}
		Global::line_num++;
		try
		{
			std::string str = interpret.ReadLine(line, !Global::read_from_file);
			if (!str.empty())
				std::cout << str << std::endl;
		}
		catch (std::exception& e)
		{
			std::cout << "Exception occured: '" << line << "': " << e.what() << std::endl;
		}
		if (!Global::read_from_file)
		{
			std::cout << ">>> ";
			std::cout.flush();
		}
	}
}
