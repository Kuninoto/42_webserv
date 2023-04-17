#include "WebServ.hpp"
#include "Server.hpp"
#include "Lexer.hpp"
#include <stack>
#include <iostream>

class ParserException : public std::exception {
    public:
        ParserException(const std::string& message) : message(message) {};
    
    virtual const char* what() const throw() {
        return message.c_str();
    };

    private:
        const std::string& message;
};

static bool areBracketsBalanced(const std::vector<Token>& token_vec)
{
    // Declare a stack to hold the previous brackets.
    std::stack<TokenType> temp;
    std::vector<Token>::const_iterator itr;

    for (itr = token_vec.begin(); itr != token_vec.end(); itr++)
    {
        if (itr->type == RIGHT_CURLY_BRACKET
        ||  itr->type == LEFT_CURLY_BRACKET)
        {
            // If the stack is empty
            // just push the current bracket
            if (temp.empty())
                temp.push(itr->type);
            else if (temp.top() == RIGHT_CURLY_BRACKET && itr->type == LEFT_CURLY_BRACKET) {
                // If we found any complete pair of bracket
                // then pop
                temp.pop();
            }
            else
                temp.push(itr->type);
        }
    }
    if (temp.empty())
        return true;
    return false;
}

std::vector<Server> parseConfigFile(std::string filename)
{
	Lexer lexer(filename);
	std::vector<Server> servers;
    std::vector<Token> tokens;
    std::stack<TokenType> brackets;

    if (!areBracketsBalanced(tokens))
        throw ParserException("Unclosed curly brackets");

    int curly_brackets = 0;

    // parseServerScope()
    Token token = lexer.nextToken();
    while (true)
    {
        if (token.value == "server")
            token = lexer.nextToken();
        if (token.type == LEFT_CURLY_BRACKET)
        {
            token = lexer.nextToken();
            curly_brackets += 1;
        }
        else if (token.type == RIGHT_CURLY_BRACKET)
        {
            token = lexer.nextToken();
            curly_brackets -= 1;
        }
        if (curly_brackets == 0 || token.type == END_OF_FILE)
        {
            servers.push_back(Server(lexer.parameters));
            lexer.parameters.clear();
            break;
        }
        tokens.push_back(token);
    }

    // loop thru tokens and if keyword == server, call createServ()

    std::cout << "SERVERS PORTS: ";
    for (size_t i = 0; i < servers.size(); i += 1)
        std::cout << servers.at(i).getPort() << " ";
    std::cout << std::endl;

	//while (token.type != END_OF_FILE)
	//{
	//	if (token.type == KEYWORD)
	//	{
	//		if (token.value == "server")
	//			servers.push_back(createServ(lexer));
	//	}
	//	token = lexer.nextToken();
	//}

	return servers;
}