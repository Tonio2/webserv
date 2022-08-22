SRCS	=	main.cpp
OBJS	=	$(SRCS:.cpp=.o)
CC		=	c++
FLAGS	=	-Wall -Werror -Wextra -std=c++98 -g

all: webserv

webserv: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o webserv

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf webserv

re: fclean all
