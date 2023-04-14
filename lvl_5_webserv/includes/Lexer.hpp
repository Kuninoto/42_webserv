#include <vector>
#include <string>
#include <iostream>
#include <fstream>

enum TokenType
{
    UNKNOWN,
    KEYWORD,
    PARAMETER,
    LEFT_CURLY_BRACKET,
    RIGHT_CURLY_BRACKET,
    END_OF_FILE,
};

struct Token
{
    TokenType type;
    std::string value;
};

class Lexer {
    public:
        Lexer(const std::string& filename);
        ~Lexer(void);
        Token nextToken(void);

        size_t line_number;

    private:
        char current_char;
        std::ifstream file;
        std::vector<std::string> keywords;
        void consumeWhiteSpace(void);
        void consumeComment(void);
        void consumeKeyword(std::string& token_value);
};
