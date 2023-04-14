#include "Lexer.hpp"

bool isValidKeyword(const std::string& value)
{
    std::string keywords[7] = {"server", "listen", "index",
                               "root", "cgi", "error_page",
                               "location"};

    return (value == keywords[0] || value == keywords[1]
        ||  value == keywords[2] || value == keywords[3]
        ||  value == keywords[4] || value == keywords[5]
        ||  value == keywords[6]);
}

Lexer::Lexer(const std::string& filename)
{
    file.open(filename.c_str());
    if (!file.is_open())
        throw "FICHEIRO NAO EXISTE";
    line_number = 1;
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
        if (isalpha(current_char)) {
            value += current_char;
            consumeKeyword(value);
            return (Token){ KEYWORD, value };
        }
        if (current_char == ' ') {
            this->current_char = file.get();
            while (!file.eof() && this->current_char != ';'
            && current_char != '\n')
            {
                value += current_char;
                this->current_char = file.get();
            }
            current_char = file.get();
            return (Token){ PARAMETER, value };
        }
        if (current_char == '{') {
            current_char = file.get();
            return (Token){ LEFT_CURLY_BRACKET, "{" };
        }
        if (current_char == '}') {
            current_char = file.get();
            return (Token){ RIGHT_CURLY_BRACKET, "}" };
        }
        // Invalid character
        value += current_char;
        current_char = file.get();
        return (Token){ UNKNOWN, value };
    }
    return (Token){ END_OF_FILE, "" };
}

void Lexer::consumeWhiteSpace(void)
{
    while (!this->file.eof() && isspace(this->current_char))
    {
        if (this->current_char == '\n') {
            this->line_number += 1;
        }
        this->current_char = file.get();
    }
}

void Lexer::consumeComment(void)
{
    while (!this->file.eof() && this->current_char != '\n')
        this->current_char = file.get();
    this->line_number += 1;
}

void Lexer::consumeKeyword(std::string& token_value)
{
    this->current_char = this->file.get();
    while (!file.eof() && (isalnum(this->current_char) || current_char == '_'))
    {
        token_value += current_char;
        this->current_char = file.get();
    }
    if (!isValidKeyword(token_value))
        throw "invalid keyword";
}

Lexer::~Lexer(void) {
    file.close();
}
