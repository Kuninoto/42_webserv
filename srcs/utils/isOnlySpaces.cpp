#include <string>

bool isOnlyWhiteSpaces(const std::string& str) {
    if (str.find_first_not_of(" \n\t\r") == std::string::npos)
        return true;
    return false;
}
