#include "Lexer.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

bool isValidKeyword(const std::string& value) {
    static const std::string keywords[16] = {"server", "listen", "host", "index",
                                             "server_name", "root", "error_page",
                                             "location", "client_max_body_size",
                                             "cgi_path", "cgi_ext", "auto_index",
                                             "allow_methods", "return", "try_file"};

    for (size_t i = 0; i < 16; i += 1) {
        if (value == keywords[i]) {
            return true;
        }
    }
    return false;
}

Lexer::Lexer(const std::string& filename) {
    file.open(filename.c_str());
    if (!file.is_open())
        throw LexerException("Failed to open config file");
    this->line_nr = 1;
    this->has_server = false;
    this->current_char = file.get();
}

Token Lexer::nextToken(void) {
    std::string value;

    while (!file.eof()) {
        if (current_char == '\n') {
            this->line_nr += 1;
            current_char = file.get();
            continue;
        }
        // Skip whitespace
        if (isspace(current_char)) {
            this->consumeWhiteSpace();
            continue;
        }
        // COMMENT
        if (current_char == '#') {
            this->consumeComment();
            continue;
        }

        if (current_char == '{') {
            current_char = file.get();
            return (Token){LEFT_CURLY_BRACKET, "{"};
        }

        if (current_char == '}') {
            current_char = file.get();
            return (Token){RIGHT_CURLY_BRACKET, "}"};
        }

        if (isalpha(current_char)) {
            value += current_char;
            current_char = this->file.get();
            consumeKeyword(value);
            return (Token){KEYWORD, value};
        }
        value += current_char;
        current_char = file.get();
    }
    return (Token){END_OF_FILE, ""};
}

void Lexer::consumeWhiteSpace(void) {
    while (!this->file.eof() && isspace(this->current_char)) {
        if (this->current_char == '\n')
            return;
        this->current_char = file.get();
    }
}

void Lexer::consumeComment(void) {
    while (!this->file.eof() && this->current_char != '\n')
        this->current_char = file.get();
}

void Lexer::consumeKeyword(std::string &token_value) {
    std::string parameter_value;

    while (!this->file.eof() && (isalnum(current_char) || current_char == '_')) {
        token_value += current_char;
        current_char = this->file.get();
    }
    if (!isValidKeyword(token_value))
        throw LexerException("line " + ft_ntos(this->line_nr) + ": unknown keyword \"" + token_value + "\"");
    if (token_value == "server") {
        this->has_server = true;
        return;
    }
    // Check for parameters
    if (current_char == ' ') {
        consumeWhiteSpace();
        if (current_char == ';')
            throw LexerException("line " + ft_ntos(this->line_nr) + ": empty parameter found for keyword \"" + token_value + "\"");
        parameter_value = "";
        if (token_value == "location") {
            while (!this->file.eof() && current_char != '{') {
                if (current_char == '\n')
                    throw LexerException("line " + ft_ntos(this->line_nr) + ": missing '{'");
                parameter_value += current_char;
                current_char = this->file.get();
            }
        } else {
            consumeWhiteSpace();
            while (!this->file.eof() && current_char != ';') {
                if (current_char == ' ' && token_value != "allow_methods")
                    throw LexerException("line " + ft_ntos(this->line_nr) + ": keyword \"" + token_value + "\" has more than one parameter");
                if (current_char == '\n')
                    throw LexerException("line " + ft_ntos(this->line_nr) + ": Missing ';'");
                parameter_value += current_char;
                current_char = this->file.get();
            }
        }
        parameters[token_value] = parameter_value;
    }
    this->current_char = this->file.get();
}

Lexer::~Lexer(void) {
    file.close();
}
