#include "WebServ.hpp"
#include "Server.hpp"
#include "Lexer.hpp"
#include <unistd.h>
#include <stack>
#include <iostream>

typedef std::pair<std::string, location_t> locationPair;
typedef std::map<std::string, location_t> locationMap;

locationPair parseLocation(const std::map<std::string, std::string>& lexerParameters,
                                                 const std::string& locationPath)
{
    location_t locationStruct;

    locationStruct.root = lexerParameters.find("root")->second;
    locationStruct.allowed_methods = splitStr(lexerParameters.find("allow_methods")->second, ' ');
    locationStruct.redirect = lexerParameters.find("return")->second;
    locationStruct.auto_index = lexerParameters.find("auto_index")->second == "on" ? true : false;
    std::string temp = lexerParameters.find("cgi_path")->second;
    if (access(temp.c_str(), X_OK) != 0)
        throw WebServ::ParserException("invalid cgi_path");
    locationStruct.cgi_path = temp;
    locationStruct.cgi_ext = lexerParameters.find("cgi_ext")->second;

    return std::make_pair<std::string, location_t>(locationPath, locationStruct);
}

static void readLocationBlock(Lexer& lexer, Token& token)
{
    token = lexer.nextToken();
    while (token.type != RIGHT_CURLY_BRACKET)
        token = lexer.nextToken();
}

std::vector<Server> parseConfigFile(std::string filename)
{
	Lexer lexer(filename);
	std::vector<Server> servers;
    size_t i = 0;

    // Parse Server scopes
    Token token = lexer.nextToken();
    while (token.type != END_OF_FILE)
    {
        bool hasLocation = false;
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
            if (token.value == "location")
            {
                if (hasLocation == false)
                {
                    servers.push_back(Server(lexer.parameters));
                    hasLocation = true;
                    lexer.parameters.clear();
                }
                readLocationBlock(lexer, token);
                servers.at(i).locations.insert(parseLocation(lexer.parameters, lexer.parameters["location"]));
                lexer.parameters.clear();
            }
            else if (curly_brackets == 0 && !hasLocation)
            {
                servers.push_back(Server(lexer.parameters));
                break;
            }            
            else if (curly_brackets == 0)
                break;
            token = lexer.nextToken();
        }
        if (curly_brackets > 0)
            throw WebServ::ParserException("Unclosed curly brackets");
        token = lexer.nextToken();
    }
	return servers;
}
