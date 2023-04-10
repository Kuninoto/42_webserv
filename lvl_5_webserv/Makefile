CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address #-O3
INCLUDE = -I./includes
VPATH = srcs srcs/utils
RM = rm -rf

GENERAL = Socket WebServ
UTILS = split_str slice_vec

NAME = webserv
TEST_CONFIG_FILE = a

SRCS = $(addsuffix .cpp, $(UTILS))\
	   $(addsuffix .cpp, $(GENERAL))\
	   main.cpp

OBJ_DIR = obj
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJS)
	$(info Compiling webserv)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	$(info Done!)
	$(info Usage: ./webserv <config_file>)

$(OBJ_DIR):
	mkdir -p obj

$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

run: all clean
	echo ""
	./$(NAME) $(TEST_CONFIG_FILE)

noleaks: re clean
	echo ""
	valgrind --leak-check=full -s --show-leak-kinds=all ./$(NAME) $(TEST_PORT) $(TEST_PASSWD)

.PHONY: all webserv clean fclean re run noleaks

.SILENT:
