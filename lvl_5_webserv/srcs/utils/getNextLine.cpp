#include <sstream>
#include <string>

std::istream& getNextLine(std::istream& ss, std::string& line) {
    line.clear();

    char c;
    while (ss.get(c)) {
        line += c;
        if (c == '\n') {
            break;
        }
    }
    return ss;
}
