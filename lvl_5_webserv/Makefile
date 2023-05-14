CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g -fsanitize=address # -Wno-unused-result -O3
INCLUDE = -I./includes
VPATH = srcs srcs/utils
RM = rm -rf

UTILS = getNextLine splitStr trimStr endsWith isOnlySpaces sliceVec time file
GENERAL = Lexer CGI Server Client WebServ

NAME = webserv
TEST_CONFIG_FILE = config/test.conf

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
	$(RM) ".output"

re: fclean all

run: all clean
	echo ""
	./$(NAME) $(TEST_CONFIG_FILE)

unit_tests: all clean
	echo "NO CONFIG FILE PROVIDED"
	./$(NAME)
	./$(NAME) ""

	echo "CONFIG FILE VALIDATION -----------"
	echo "NO EXTENSION"
	./$(NAME) config/error/foo

	ecgo "UNEXISTENT FILE"
	./$(NAME) config/error/foobar.conf

	echo "WRONG EXTENSION"
	./$(NAME) config/error/foo.txt

	echo "EMPTY FILE"
	./$(NAME) config/error/empty.conf

	echo "UNEVEN BRACKETS"
	./$(NAME) config/error/uneven_brackets.conf

	echo "EMPTY SERVER BLOCK"
	./$(NAME) config/error/empty_server.conf

	echo "INVALID PORT NUMBER"
	./$(NAME) config/error/invalid_port.conf

	echo "INVALID CLIENT_MAX_BODY_SIZE"
	./$(NAME) config/error/invalid_client_max_body_size.conf

	echo "KEYWORD WITHOUT VALUE"
	./$(NAME) config/error/keyword_without_value.conf

	echo "MORE THAN ONE VALUE FOR A KEYWORD"
	./$(NAME) config/error/more_than_one_value.conf


noleaks: re clean
	echo ""
	valgrind --leak-check=full -s --show-leak-kinds=all ./$(NAME) $(TEST_PORT) $(TEST_PASSWD)

.PHONY: all webserv clean fclean re run noleaks

.SILENT:

.IGNORE:
