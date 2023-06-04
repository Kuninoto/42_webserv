#ifndef LIBWEBSERV_HPP
#define LIBWEBSERV_HPP

#include <memory.h>
#include <poll.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <sys/epoll.h>
#include <memory.h>
#include <vector>

#include "CGI.hpp"
#include "Client.hpp"
#include "Lexer.hpp"
#include "Server.hpp"
#include "WebServ.hpp"
#include "utils.hpp"

#define ERROR_MSG_PREFFIX "webserv: error: "
#define NO_CONFIG_FILE_ERR "no configuration file provided"
#define ARGC_ERR "invalid number of arguments"

/**
 * @brief Writes <error_msg> in the cerr stream followed by an endl.
 * Serves as a wrapper to write an error message and return EXIT_FAILURE
 *
 * @return 1, representing a failure exit status
 */
static inline int panic(const std::string &error_msg) {
    std::cerr << ERROR_MSG_PREFFIX << error_msg << std::endl;
    return EXIT_FAILURE;
}

#endif  // LIBWEBSERV_HPP