#include <map>
#include <string>
#include <fstream>

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
                LexerException(const std::string& message) : message(message) {};

                virtual const char* what() const throw() {
                    return message.c_str();
                };
            private:
                const std::string& message;
        };

    private:
        std::ifstream file;
        char current_char;
        void consumeWhiteSpace(void);
        void consumeComment(void);
        void consumeKeyword(std::string& token_value);
        void consumeParameter(std::string& token_value);
};
