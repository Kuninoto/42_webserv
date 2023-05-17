#ifndef UTILS_HPP
#define UTILS_HPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define RESET "\x1B[0m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define PURPLE "\x1B[35m"

std::istream& getNextLine(std::istream& ss, std::string& line);

std::vector<std::string> splitStr(const std::string& s, char delimiter);
void trimStr(std::string& str, const char* set);

bool endsWith(const std::string& fullString, const std::string& ending);
bool isOnlyWhiteSpaces(const std::string& str);

template <typename T>
std::vector<T> sliceVec(std::vector<T>& vec, unsigned int start_idx, unsigned int end_idx);

const std::string getTimeStamp(void);
const std::string getTime(void);

bool isRegularFile(const char* path);
bool isDirectory(const char* path);
std::string getFileContent(const std::string& file);
std::string getFileType(const std::string& file);
std::string getFileSize(const std::string& file);
std::string getOkHeader(const std::string& file);
std::string getHTMLBoilerPlate(const std::string& code, const std::string& title, const std::string& body);

static inline void logMessage(const std::string& message) {
    std::cout << BLUE << "[" << getTime() << "] " << RESET << message << RESET << std::endl;
};

template <typename T>
std::string ft_ntos(const T num) {
    std::string as_str;
    std::stringstream ss;

    ss << num;
    ss >> as_str;
    return as_str;
}

size_t ft_stoul(const std::string& num_as_str);

#endif  // UTILS_HPP