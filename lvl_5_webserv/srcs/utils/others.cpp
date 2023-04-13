#include "utils.hpp"

bool	emptyStr(std::string line)
{
	for (std::string::const_iterator it = line.begin(); it != line.end(); ++it)
	{
		if (*it != ' ' || *it != '\n')
			return false;
	}
	return true;
}