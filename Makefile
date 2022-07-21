all:
	c++ -Wall -Werror -Wextra -std=c++98 -g main.cpp request.cpp response.cpp -o webserv