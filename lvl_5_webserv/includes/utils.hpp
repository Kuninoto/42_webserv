#ifndef UTILS_HPP
# define UTILS_HPP

# include <vector>
# include <string>

std::vector<std::string> splitStr(const std::string& s, char delimiter);

template <typename T>
std::vector<T> sliceVec(std::vector<T>& vec, unsigned int start_idx, unsigned int end_idx);

const std::string getTimeStamp(void);

#endif // UTILS_HPP