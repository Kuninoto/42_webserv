#include "utils.hpp"

std::vector<std::string> splitStr(std::string& str, const std::string& delimiter)
{
    std::vector<std::string> v;

    if (!str.empty())
	{
        int start = 0;

        while (true)
		{
            size_t idx = str.find(delimiter, start);
            if (idx == std::string::npos) {
                break;
            }
 
            int length = (idx - start);
            v.push_back(str.substr(start, length));
            start += length + delimiter.size();
		}
        v.push_back(str.substr(start));
    }
	return v;
}
