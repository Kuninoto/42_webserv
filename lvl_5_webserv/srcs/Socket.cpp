#include "Socket.hpp"

#include <iostream>
using std::cout;
using std::endl;

Socket::Socket(uint16_t port)
: port(port) {};

Socket::Socket(Socket &copy)
: socket_fd(copy.socket_fd), port(copy.port)
{};

Socket::~Socket(void) {};

bool Socket::setSocketFd(void)
{
	this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->socket_fd == -1)
		return false;
	cout << "Socket's FD = " << this->socket_fd << endl;
	return true;
};

uint16_t Socket::getPort(void) const {
	return this->socket_fd;
};

int Socket::getSocketFd(void) const {
	return this->socket_fd;
};
