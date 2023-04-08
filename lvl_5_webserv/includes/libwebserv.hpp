#ifndef LIBWEBSERV_HPP
# define LIBWEBSERV_HPP

# include <cstdlib>
# include <string>
# include <iostream>

# define ERROR_MSG_PREFFIX "webserv: error: "
# define ARGS_ERR "invalid arguments"

/**
 * @brief Writes <error_msg> in the cerr stream followed by an endl.
 * Serves as a wrapper to write an error message and return EXIT_FAILURE
 *
 * @return 1, representing a failure exit status
 */
static inline int panic(const std::string &error_msg)
{
    std::cerr << ERROR_MSG_PREFFIX << error_msg << std::endl;
    return EXIT_FAILURE;
}

#endif // LIBWEBSERV_HPP