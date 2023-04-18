#include "WebServ.hpp"
#include "Server.hpp"
#include "Lexer.hpp"
#include <stack>
#include <iostream>

std::vector<Server> parseConfigFile(std::string filename)
{
	Lexer lexer(filename);
	std::vector<Server> servers;

    // Parse Server scopes
    Token token = lexer.nextToken();
    while (token.type != END_OF_FILE)
    {
        int curly_brackets = 0;
        while (true)
        {
            if (token.value == "server")
            {
                token = lexer.nextToken();
                if (token.type != LEFT_CURLY_BRACKET)
                    throw WebServ::ParserException("no opening bracket for server");
            }
            if (token.type == LEFT_CURLY_BRACKET) {
                curly_brackets += 1;
            }
            else if (token.type == RIGHT_CURLY_BRACKET) {
                curly_brackets -= 1;
            }
            //if (token.value == "location")
            //{
            // different behavior    
            //}
            if (curly_brackets == 0 || token.type == END_OF_FILE)
            {
                servers.push_back(Server(lexer.parameters));
                lexer.parameters.clear();
                break;
            }
            token = lexer.nextToken();
        }
        if (curly_brackets > 0)
            throw WebServ::ParserException("Unclosed curly brackets");
        token = lexer.nextToken();
    }

    for (size_t i = 0; i < servers.size(); i += 1) {
        std::cout << servers.at(i) << std::endl; 
    }

	return servers;
}