
all:
	g++ *.cpp -I . -std=c++14 -fsanitize=address -fsanitize=undefined -g -Wall -Werror
