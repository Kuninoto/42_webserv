#include <string>

bool endsWith(const std::string& fullString, const std::string& ending) {
    if (fullString.length() < ending.length()) {
        return false;
    }
    return fullString.compare(fullString.length() - ending.length(), ending.length(), ending) == 0;
}
