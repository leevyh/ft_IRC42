NAME = ircserv
NAME_BONUS = ircserv_bonus

PATH_SRCS = srcs
PATH_SRCS_BONUS = srcs_bonus
PATH_INCLUDE = includes


CXX = c++ -std=c++98
CXXFLAGS = -Wall -Wextra -Werror -g3


HDRS = $(PATH_INCLUDE)/IRC.hpp \
		$(PATH_INCLUDE)/Server.hpp \
		$(PATH_INCLUDE)/User.hpp \
		$(PATH_INCLUDE)/Commands.hpp \
		$(PATH_INCLUDE)/Channel.hpp

SRCS = $(PATH_SRCS)/main.cpp \
		$(PATH_SRCS)/Server.cpp \
		$(PATH_SRCS)/utils.cpp \
		$(PATH_SRCS)/User.cpp \
		$(PATH_SRCS)/Commands.cpp \
		$(PATH_SRCS)/RPL.cpp \
		$(PATH_SRCS)/Channel.cpp \
		$(PATH_SRCS)/Mode.cpp

SRCS_BONUS = $(PATH_SRCS_BONUS)/bot.cpp \
			 $(PATH_SRCS_BONUS)/main_bonus.cpp \

OBJ_DIR = .objs
OBJ_DIR_BONUS = .objs_bonus

OBJS = $(patsubst $(PATH_SRCS)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
OBJ_BONUS = $(patsubst $(PATH_SRCS_BONUS)/%.cpp, $(OBJ_DIR_BONUS)/%.o, $(SRCS_BONUS))

$(OBJ_DIR)/%.o: $(PATH_SRCS)/%.cpp $(HDRS)
	@mkdir -p $(OBJ_DIR) $(OBJ_DIR)/$(PATH_SRCS)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I $(PATH_INCLUDE) -I $(PATH_SRCS)

$(OBJ_DIR_BONUS)/%.o: $(PATH_SRCS_BONUS)/%.cpp $(HDRS)
	@mkdir -p $(OBJ_DIR_BONUS) $(OBJ_DIR_BONUS)/$(PATH_SRCS_BONUS)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I $(PATH_INCLUDE) -I $(PATH_SRCS_BONUS)

all: $(NAME)

bonus: $(NAME_BONUS)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
	@echo "\n Mandatory compilation: OK\n"

$(NAME_BONUS) : $(OBJ_BONUS) 
	$(CXX) $(CXXFLAGS) -o $(NAME_BONUS) $(OBJS) $(OBJ_BONUS) 
	@echo "\n Compilation Bonus OK\n"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)
	rm -f $(OBJ_BONUS)

fclean: clean
	rm -f $(NAME)
	rm -f $(NAME_BONUS)
	rm  -rf $(OBJ_DIR)
	rm	-rf $(OBJ_DIR_BONUS)

re: fclean
	@$(MAKE) --no-print-dir

log: fclean
	rm -f *.log


.PHONY: all clean fclean re
