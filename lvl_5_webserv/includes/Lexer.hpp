#ifndef LEXER_HPP
#define LEXER_HPP

#include <fstream>
#include <map>
#include <string>

#define OPEN_FILE_ERR "failed to open config file"

enum TokenType {
    KEYWORD,
    LEFT_CURLY_BRACKET,
    RIGHT_CURLY_BRACKET,
    END_OF_FILE,
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
   public:
    Lexer(const std::string& filename);
    ~Lexer(void);
    Token nextToken(void);

    // Keyword, parameter
    std::map<std::string, std::string> parameters;

    class LexerException : public std::exception {
       public:
        std::string message;
        LexerException(std::string message) : message("config file: " + message) {};
        ~LexerException() throw() {};
        virtual const char* what() const throw() {
            return message.c_str();
        };
    };

   private:
    int brackets;
    bool has_server;
    size_t lineNr;
    std::ifstream file;
    char current_char;
    void consumeWhiteSpace(void);
    void consumeComment(void);
    void consumeKeyword(std::string& token_value);
};

#endif  // LEXER_HPP