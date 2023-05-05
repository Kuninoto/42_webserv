#include <string>

void trimStr(std::string& str, const char* set) {
    str.erase(0, str.find_first_not_of(set));
    str.erase(str.find_last_not_of(set) + 1);
}
