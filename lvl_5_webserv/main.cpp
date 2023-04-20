#include "libwebserv.hpp"

# include <iostream>
# include <stdio.h>
# include <netdb.h>

using std::cout;
using std::cerr;
using std::endl;

# define NR_PENDING_CONNECTIONS 10

bool g_stopServer = false;

int main(int argc, char **argv)
{
	(void)argc;
	std::vector<Server> servers = parseConfigFile(argv[1]);

	for (size_t i = 0; i < servers.size(); i += 1) {
        std::cout << servers.at(i) << std::endl; 
    }

	/* (void)argc;
	Lexer lexer(argv[1]);
	Token token;
	while (true)
	{
	   token = lexer.nextToken();
	   switch (token.type) {
	       case KEYWORD:
	           std::cout << "Keyword: " << token.value << std::endl;
	           std::cout << "Parameter: \"" << lexer.parameters[token.value] << "\"" << std::endl;
	           break;
			case LEFT_CURLY_BRACKET:
	           std::cout << "LEFT CURLY BRACKET" << std::endl;
	           break;
			case RIGHT_CURLY_BRACKET:
	           std::cout << "RIGHT CURLY BRACKET" << std::endl;
	           break;
	       case END_OF_FILE:
	           std::cout << "End of file" << std::endl;
	           break;
	   }
		if (token.type == END_OF_FILE)
			break;
	}; */
	return EXIT_SUCCESS;
}
