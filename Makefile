NAME = ircserv


PATH_SRCS = srcs
PATH_INCLUDE = includes


CXX = c++ -std=c++98
CXXFLAGS = -Wall -Wextra -Werror -g3


HDRS = $(PATH_INCLUDE)/IRC.hpp \
		$(PATH_INCLUDE)/Server.hpp \
		$(PATH_INCLUDE)/User.hpp \
		$(PATH_INCLUDE)/Commands.hpp \

SRCS = $(PATH_SRCS)/main.cpp \
		$(PATH_SRCS)/Server.cpp \
		$(PATH_SRCS)/utils.cpp \
		$(PATH_SRCS)/User.cpp \
		$(PATH_SRCS)/Commands.cpp \
		$(PATH_SRCS)/RPL.cpp


OBJ_DIR = .objs

OBJS = $(patsubst $(PATH_SRCS)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

$(OBJ_DIR)/%.o: $(PATH_SRCS)/%.cpp $(HDRS)
	@mkdir -p $(OBJ_DIR) $(OBJ_DIR)/$(PATH_SRCS)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I $(PATH_INCLUDE) -I $(PATH_SRCS)


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
