#include "WebServ.hpp"
#include "Server.hpp"
#include "Lexer.hpp"
#include <unistd.h>
#include <stack>
#include <iostream>
#include <memory.h>

locationPair parseLocation(const std::map<std::string, std::string>& lexerParameters,
                                                 std::string& locationPath)
{
    location_t locationStruct;
    bzero(&locationStruct, sizeof(location_t));

    if (lexerParameters.count("root") > 0)
        locationStruct.root = lexerParameters.find("root")->second;
    if (lexerParameters.count("allow_methods") > 0)
        locationStruct.allowed_methods = splitStr(lexerParameters.find("allow_methods")->second, ' ');
    if (lexerParameters.count("return") > 0)
        locationStruct.redirect = lexerParameters.find("return")->second;
    if (lexerParameters.count("auto_index") > 0)
        locationStruct.auto_index = lexerParameters.find("auto_index")->second == "on" ? true : false;
    if (lexerParameters.count("try_file") > 0)
        locationStruct.try_file = lexerParameters.find("try_file")->second;
    if (lexerParameters.count("cgi_path") > 0)
    {
        std::string temp = lexerParameters.find("cgi_path")->second;
        if (access(temp.c_str(), X_OK) != 0) {
            throw WebServ::ParserException("invalid cgi_path");
        }
        locationStruct.cgi_path = temp;
    }
    if (lexerParameters.count("cgi_ext") > 0)
        locationStruct.cgi_ext = lexerParameters.find("cgi_ext")->second;
    trimStr(locationPath, " ");
    return std::make_pair<std::string, location_t>(locationPath, locationStruct);
}

static void readLocationBlock(Lexer& lexer, Token& token)
{
    token = lexer.nextToken();
    while (token.type != RIGHT_CURLY_BRACKET) {
        token = lexer.nextToken();
    }
}

std::vector<Server> parseConfigFile(std::string filename)
{
	Lexer lexer(filename);
	std::vector<Server> servers;

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
                    hasLocation = true;
                    servers.push_back(Server(lexer.parameters));
                    std::string temp = lexer.parameters["location"];
                    lexer.parameters.clear();
                    lexer.parameters["location"] = temp;
                }
                readLocationBlock(lexer, token);
                servers.back().locations.insert(parseLocation(lexer.parameters, lexer.parameters["location"]));
                lexer.parameters.clear();
            }
            else if (curly_brackets == 0 && !hasLocation)
            {
                servers.push_back(Server(lexer.parameters));
                break;
            }            
            else if (curly_brackets == 0) {
                break;
            }
            token = lexer.nextToken();
        }
        if (curly_brackets > 0)
            throw WebServ::ParserException("Unclosed curly brackets");
        token = lexer.nextToken();
    }
	return servers;
}
