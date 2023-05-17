#include <sstream>
#include <string>

size_t ft_stoul(const std::string& num_as_str) {
    size_t nbr;
    std::stringstream ss(num_as_str);

    ss >> nbr;
    return nbr;
}
