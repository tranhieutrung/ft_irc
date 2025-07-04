NAME 		=	ircserv

HEADER		=	./includes

SRC_DIR		=	./srcs

OBJ_DIR		=	./objs

SRC_FILES	=	main.cpp \
				Server.cpp \
				User.cpp \
				Channel.cpp \
				commands.cpp \
				IO.cpp \
				ChannelCommands.cpp \
				sendMessage.cpp	\
				Utils.cpp

SRCS		=	$(addprefix $(SRC_DIR)/, $(SRC_FILES))

OBJS		=	$(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Compiler and flags
CXX 		=	c++
CXXFLAGS 	=	-Wall -Wextra -Werror -std=c++20 -g
RM			=	rm -rf

# Targets
all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -I$(HEADER) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
