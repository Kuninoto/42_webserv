#include <sys/stat.h>

#include <ctime>
#include <iostream>
#include <string>

// Weekday, day month year hour:minute:second GMT
const std::string getTimeStamp(void) {
    time_t now = time(0);
    struct tm* tm_time = localtime(&now);
    char buffer[80] = {0};

    strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", tm_time);

    return std::string(buffer);
}

// Hour:minute:second
const std::string getTime(void) {
    time_t now = time(0);
    struct tm* tm_time = localtime(&now);
    char buffer[9] = {0};

    strftime(buffer, 9, "%H:%M:%S", tm_time);
    return std::string(buffer);
}
