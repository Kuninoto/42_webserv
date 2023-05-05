#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> splitStr(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        if (!token.empty())
            tokens.push_back(token);
    }
    return tokens;
}
