#include "Socket.hpp"

using std::cout;
using std::endl;

Socket::Socket(uint16_t port)
: port(port) {};

Socket::Socket(Socket &copy)
: socket_fd(copy.socket_fd), port(copy.port) {};

Socket::~Socket(void) {};

bool Socket::setSocketFd(void)
{
	this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	return this->socket_fd != -1;
};
