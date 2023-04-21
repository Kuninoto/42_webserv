#include <fstream>
#include <sstream>
#include "Lexer.hpp"

#include <iostream>

bool isValidKeyword(const std::string& value)
{
    static const std::string keywords[15] = {"server", "listen", "host", "index", 
                                           "server_name", "root", "error_page",
                                           "location", "client_max_body_size",
                                           "cgi_path", "cgi_ext", "auto_index",
                                           "allow_methods", "return"};

    for (size_t i = 0; i < 14; i += 1) {
        if (value == keywords[i]) {
            return true;
        }
    }
    return false;
}

Lexer::Lexer(const std::string& filename)
{
    file.open(filename.c_str());
    if (!file.is_open())
        throw LexerException("Failed to open config file");
    current_char = file.get();
}

Token Lexer::nextToken(void)
{
    std::string value;

    // Skip whitespace and comments
    while (!file.eof())
    {
        if (isspace(current_char)) {
            consumeWhiteSpace();
            continue;
        }
        // COMMENT
        if (current_char == '#') {
            consumeComment();
            continue;
        }
        if (current_char == '{') {
            current_char = file.get();
            return (Token){ LEFT_CURLY_BRACKET, "{" };
        }
        if (current_char == '}') {
            current_char = file.get();
            return (Token){ RIGHT_CURLY_BRACKET, "}" };
        }
        if (isalpha(current_char)) {
            value += current_char;
            consumeKeyword(value);
            return (Token){ KEYWORD, value};
        }
        value += current_char;
        current_char = file.get();
    }
    return (Token){ END_OF_FILE, "" };
}

void Lexer::consumeWhiteSpace(void)
{
    while (!this->file.eof() && isspace(this->current_char))
        this->current_char = file.get();
}

void Lexer::consumeComment(void)
{
    while (!this->file.eof() && this->current_char != '\n')
        this->current_char = file.get();
}

void Lexer::consumeKeyword(std::string& token_value)
{
    current_char = this->file.get();
    std::string parameter_value;

    while (!this->file.eof() && (isalnum(current_char) || current_char == '_')) {
        token_value += current_char;
        current_char = this->file.get();
    }
    if (!isValidKeyword(token_value))
        throw LexerException("Unknown keyword \"" + token_value + "\"");
    if (token_value == "server")
        return ;
    // Check for parameters
    if (current_char == ' ')
    {
        consumeWhiteSpace();
        parameter_value = "";
        if (token_value == "location")
        {
            while (!this->file.eof() && current_char != '{')
            {
                parameter_value += current_char;
                current_char = this->file.get();
            }
        }
        else
        {
            while (!this->file.eof() && current_char != ';')
            {
                if (current_char == ' ' && token_value != "allow_methods")
                    throw LexerException("Keyword \"" + token_value + "\" has more than one parameter");
                parameter_value += current_char;
                current_char = this->file.get();
            }
        }
        if (parameter_value.empty())
            throw LexerException("Empty parameter found for keyword \"" + token_value + "\"");
        parameters[token_value] = parameter_value;
    }
    this->current_char = this->file.get();
}

Lexer::~Lexer(void) {
    file.close();
}
