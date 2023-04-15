#include "WebServ.hpp"
#include "Lexer.hpp"
#include <stack>
#include <stdlib.h>
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

/**
 * @brief Converts the string representation of the port to its correspondent integer
 * @param port_as_str string representation of the port number
 * @return On success: port number
 * On error: -1 (invalid port number)
 */
static int parsePortNumber(const std::string& port_as_str)
{
	unsigned long temp = strtoul(port_as_str.c_str(), NULL, 10);
	bool strtoul_success = (temp == UINT64_MAX && errno == ERANGE) ? false : true;

	if (port_as_str.find_first_not_of("0123456789") != std::string::npos 
	|| !strtoul_success || temp > UINT16_MAX)
		return -1;
	return (int)temp;
}

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

static WebServ createServ(Lexer& lexer)
{
	WebServ	sv;
	Token 	token;

	if (token.value == "listen")
	{
		int tempPort = parsePortNumber(lexer.parameters["listen"]);
		if (tempPort == -1)
			throw ParserException("Invalid port number");
		sv.setPort(tempPort);
	}
	return sv;
}

std::vector<WebServ&> parseConfigFile(std::string filename)
{
	Lexer lexer(filename);
	std::vector<WebServ&> servers;
    std::vector<Token> tokens;
    std::stack<TokenType> brackets;

    if (!areBracketsBalanced(tokens))
        throw ParserException("Unclosed curly brackets");
    for (Token token = lexer.nextToken(); token.type != END_OF_FILE; token = lexer.nextToken()) {
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