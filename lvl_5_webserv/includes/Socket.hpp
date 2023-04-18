#ifndef SOCKET_HPP 
# define SOCKET_HPP

# define SOCKET_OPEN_ERR "fatal: failed to open socket"
# define SETSOCKETOPT_ERR "fatal: settockopt() failed"
# define BINDING_ERR "fatal: failed to bind"

# include <stdint.h>
# include <sys/socket.h>
# include <unistd.h>
# include <iostream>

class Socket {
	public:
		Socket(uint16_t port);
		Socket(Socket &copy);
		~Socket(void);

		bool setSocketFd(void);

		uint16_t getPort(void) const { return this->port; };
		int getSocketFd(void) const { return this->socket_fd; };;

	private:
		int	socket_fd;
		const uint16_t port;

		Socket(void);
};

#endif // SOCKET_HPP