#include "utils.hpp"

static int len(const std::string& s, const std::string& splitter)
{
	size_t		i = 0;
	size_t		ret = 0;

	if (splitter.empty())
		return 2;
	while (ret < s.length())
	{
		ret = s.find(splitter, ret);
		if (ret == std::string::npos)
			break;
		ret += splitter.length();
		i += 1;
	}
	return i + 2;
}

std::vector<std::string> split_str(const std::string& s, const std::string& splitter)
{
	std::vector<std::string>    now;
	std::string					tmp;
	size_t						ret = 0;
	size_t						k = 0;

	if (splitter.empty())
	{
		now.push_back("");
		return now;
	}
	while (ret < s.length())
	{
		k = ret;
		ret = s.find(splitter, ret);
		if (k != ret)
		{
			if (ret == std::string::npos)
				now.push_back(s.substr(k));
			else
			{
				now.push_back(s.substr(k));
				now.back().resize(ret - k);
			}
		}
		if (ret == std::string::npos)
			break;
		ret += splitter.length();
	}
	return now;
}