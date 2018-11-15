
all:
	g++ *.cpp -o computorv2 -I . -std=c++14 -Wall -Werror
clean:
	rm computorv2
