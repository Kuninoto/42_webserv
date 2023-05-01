#include <ctime>
#include <sys/stat.h>
#include <string>
#include <iostream>

const std::string getTimeStamp(void)
{
	time_t now = time(0);
	struct tm* tm_time = localtime(&now);
	char buffer[80];

	strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", tm_time);

	return std::string(buffer);
}

const std::string getTime(void)
{
    time_t now = time(0);
    struct tm* tm_time = localtime(&now);
    char buffer[9];

    strftime(buffer, 9, "%H:%M:%S", tm_time);
    return std::string(buffer);
}
