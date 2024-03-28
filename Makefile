NAME = ircserv

SRC = srcs/main.cpp \

OBJS = $(SRC:.cpp=.o)

CXX = c++ -std=c++98
CXXFLAGS = -Wall -Wextra -Werror -g3

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
	@echo "\n Mandatory compilation: OK\n"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean
	@$(MAKE) --no-print-dir

.PHONY: all clean fclean re
