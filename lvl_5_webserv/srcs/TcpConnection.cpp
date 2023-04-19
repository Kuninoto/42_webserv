#include "TcpConnection.hpp"

TcpConnection::TcpConnection(const std::string& server, const std::string& port) : m_server(server), m_port(port), m_socket(-1)
{
	struct addrinfo hints, *servinfo, *p;
	int status;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(m_server.c_str(), m_port.c_str(), &hints, &servinfo)) != 0) {
		std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
		return;
	}

	// loop through all the results and connect to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((m_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("socket");
			continue;
		}

		if (connect(m_socket, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(m_socket);
			perror("connect");
			continue;
		}
		break;
	}

	if (p == NULL) {
		std::cerr << "failed to connect" << std::endl;
		return;
	}

	freeaddrinfo(servinfo);
}

TcpConnection::~TcpConnection() {
	close(m_socket);
}

ssize_t TcpConnection::send(const std::string& data) {
	return ::send(m_socket, data.c_str(), data.length(), 0);
}

ssize_t TcpConnection::recv(std::string& data, size_t size)
{
	char buf[size];
	ssize_t num_bytes = ::recv(m_socket, buf, size - 1, 0);
	if (num_bytes == -1) {
		std::cerr << "recv error: " << strerror(errno) << std::endl;
		return -1;
	} else if (num_bytes == 0) {
		std::cerr << "connection closed by server" << std::endl;
		return 0;
	} else {
		buf[num_bytes] = '\0';
		data = buf;
		return num_bytes;
	}
}