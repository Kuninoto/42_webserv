#include "utils.hpp"
#include <ctime>

const std::string getTimeStamp(void)
{
    time_t now = time(0);
    struct tm* tm_time = localtime(&now);
    char buffer[80];

    strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", tm_time);

    return std::string(buffer);
}
