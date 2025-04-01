CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDE = -I./includes
VPATH = srcs srcs/utils
RM = rm -rf

UTILS = getNextLine splitStr trimStr endsWith isOnlySpaces sliceVec time file ft_stoul
GENERAL = Lexer CGI Server Client WebServ

NAME = webserv

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
	./$(NAME)

unit_tests: all clean
	echo "CONFIG FILE VALIDATION -----------"
	echo "NO EXTENSION"
	./$(NAME) config/error/foo

	echo ""

	echo "UNEXISTENT FILE"
	./$(NAME) config/error/foobar.conf

	echo ""
 
	echo "WRONG EXTENSION"
	./$(NAME) config/error/foo.txt

	echo ""
 
	echo "EMPTY FILE"
	./$(NAME) config/error/empty.conf

	echo ""
 
	echo "UNEVEN BRACKETS"
	./$(NAME) config/error/uneven_brackets.conf

	echo ""

	echo "MISSING SEMICOLON"
	./$(NAME) config/error/missing_semicolon.conf

	echo ""
 
	echo "EMPTY SERVER BLOCK"
	./$(NAME) config/error/empty_server.conf

	echo ""
 
	echo "INVALID PORT NUMBER"
	./$(NAME) config/error/invalid_port.conf

	echo ""
 
	echo "INVALID CLIENT_MAX_BODY_SIZE"
	./$(NAME) config/error/invalid_client_max_body_size.conf

	echo ""
 
	echo "KEYWORD WITHOUT VALUE"
	./$(NAME) config/error/keyword_without_value.conf

	echo ""
 
	echo "MORE THAN ONE VALUE FOR A KEYWORD"
	./$(NAME) config/error/more_than_one_value.conf

.PHONY: all webserv clean fclean re run noleaks

.SILENT:

.IGNORE:
