#include "Lexer.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#include "utils.hpp"

bool isValidKeyword(const std::string& value) {
    static const std::string keywords[16] = {"server", "listen", "host", "index",
                                             "server_name", "root", "error_page",
                                             "location", "client_max_body_size",
                                             "cgi_path", "cgi_ext", "upload_to",
                                             "auto_index", "allow_methods",
                                             "return", "try_file"};

    for (size_t i = 0; i < 17; i += 1) {
        if (value == keywords[i]) {
            return true;
        }
    }
    return false;
}

Lexer::Lexer(const std::string& filename) {
    size_t dotPos = filename.find_last_of(".");
    if (dotPos == std::string::npos)
        throw LexerException("no extension");

    std::string extension = filename.substr(dotPos);
    if (extension != ".conf")
        throw LexerException("unknown extension \"" + extension + "\"; expected \".conf\"");

    this->file.open(filename.c_str());
    if (!this->file.is_open())
        throw LexerException(OPEN_FILE_ERR);
    this->lineNr = 1;
    this->brackets = 0;
    this->has_server = false;
    this->current_char = this->file.get();
}

Token Lexer::nextToken(void) {
    std::string value;

    while (!file.eof()) {
        if (current_char == '\n') {
            this->lineNr += 1;
            current_char = file.get();
            continue;
        }
        // Skip whitespace
        if (isspace(current_char)) {
            this->consumeWhiteSpace();
            continue;
        }
        // Skip comments
        if (current_char == '#') {
            this->consumeComment();
            continue;
        }

        if (current_char == '{') {
            this->brackets += 1;
            current_char = file.get();
            return (Token){LEFT_CURLY_BRACKET, "{"};
        }

        if (current_char == '}') {
            this->brackets -= 1;
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
    if (this->brackets != 0)
        throw LexerException("uneven curly brackets");
    if (this->has_server == false)
        throw LexerException("no server block was found");
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

void Lexer::consumeKeyword(std::string& token_value) {
    std::string parameter_value;

    while (!this->file.eof() && (isalnum(current_char) || current_char == '_')) {
        token_value += current_char;
        current_char = this->file.get();
    }
    if (!isValidKeyword(token_value))
        throw LexerException("line " + ft_ntos(this->lineNr) + ": unknown keyword \"" + token_value + "\"");
    if (token_value == "server") {
        this->has_server = true;
        return;
    }
    // Check for values
    if (current_char == ' ') {
        consumeWhiteSpace();
        if (current_char == ';')
            throw LexerException("line " + ft_ntos(this->lineNr) + ": no value found for keyword \"" + token_value + "\"");
        parameter_value = "";
        if (token_value == "location") {
            while (!this->file.eof() && current_char != '{') {
                if (current_char == '\n')
                    throw LexerException("line " + ft_ntos(this->lineNr) + ": missing '{'");
                parameter_value += current_char;
                current_char = this->file.get();
                if (current_char == '{')
                    this->brackets += 1;
            }
            if (parameter_value.empty() || isOnlyWhiteSpaces(parameter_value))
                throw LexerException("line " + ft_ntos(this->lineNr) + ": no value found for keyword \"" + token_value + "\"");
        } else {
            consumeWhiteSpace();
            while (!this->file.eof() && current_char != ';') {
                if (current_char == ' ' && token_value != "allow_methods")
                    throw LexerException("line " + ft_ntos(this->lineNr) + ": keyword \"" + token_value + "\" has more than one value");
                if (current_char == '\n')
                    throw LexerException("line " + ft_ntos(this->lineNr) + ": Missing ';'");
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
