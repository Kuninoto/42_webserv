#ifndef UTILS_HPP
# define UTILS_HPP

# include <vector>
# include <string>
# include <iostream>

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

std::string getHeader(std::string header);

static inline void messageLog(std::string message, std::string color, bool error)
{
    if (error) std::cerr << color << "["<< getTime() << "] " << message << std::endl;
    else std::cout << color << "["<< getTime() << "] " << message << std::endl;
};

#endif // UTILS_HPP