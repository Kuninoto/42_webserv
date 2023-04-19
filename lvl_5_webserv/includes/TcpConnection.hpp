#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

class TcpConnection {
public:
	TcpConnection(const std::string& server, const std::string& port);
	~TcpConnection();

	ssize_t send(const std::string& data);
	ssize_t recv(std::string& data, size_t size);

private:
	std::string	m_server;
	std::string	m_port;
	int			m_socket;
};
