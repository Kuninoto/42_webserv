#ifndef UTILS_HPP
# define UTILS_HPP

# include <vector>
# include <string>
# include <iostream>
# include <sstream>

# define RESET "\x1B[0m"
# define RED "\x1B[31m"
# define GREEN "\x1B[32m"
# define YELLOW "\x1B[33m"
# define BLUE "\x1B[34m"
# define PURPLE "\x1B[35m"

std::vector<std::string> splitStr(const std::string& s, char delimiter);
void trimStr(std::string& str, const char *set);

template <typename T>
std::vector<T> sliceVec(std::vector<T>& vec, unsigned int start_idx, unsigned int end_idx);

const std::string getTimeStamp(void);
const std::string getTime(void);

bool isRegularFile(const char *path);
bool isDirectory(const char *path);
std::string getFileType(std::string file);
std::string getFileSize(std::string file);
std::string getHeader(std::string file, std::string& code);
std::string getResponseBoilerPlate(const std::string& code, const std::string& title, const std::string& body); 

static inline void messageLog(const std::string& message, const char *color, bool error)
{
    if (error) std::cerr << color << "["<< getTime() << "] " << message << RESET << std::endl;
    else std::cout << color << "["<< getTime() << "] " << message << RESET << std::endl;
};

template <typename T>
static std::string ft_ntos(const T num)
{
    std::string as_str;
    std::stringstream ss;

    ss << num;
    ss >> as_str;
    return as_str;
}

#endif // UTILS_HPP