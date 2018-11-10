
#include "Interpreter.hpp"
#include <iostream>
#include <string>

int	main(int ac, char** av)
{
	Interpreter interpret;
	for (std::string line; std::getline(std::cin, line);)
	{
		try
		{
			interpret.ReadLine(line);
		}
		catch (std::exception& e)
		{
			std::cout << "Exception occured: " << e.what() << std::endl;
		}
	}
}
