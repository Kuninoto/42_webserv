#include "WebServ.hpp"

# include <iostream>
# include <stdlib.h>
# include <signal.h>
# include <string.h>
# include <netdb.h>
# include <sys/types.h>

# define NR_PENDING_CONNECTIONS 10

extern bool g_stopServer;

static void closeServer(int signum)
{
	(void) signum;
	messageLog("Closing server", YELLOW, false);
	g_stopServer = true;
}

WebServ::WebServ(void)
{
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, closeServer);
};

void WebServ::bootServers(std::vector<Server> &servers, struct pollfd **pollfds)
{

	std::vector<Server>::iterator server;
	struct addrinfo hints, *result;
	int opt = 1;
	int i = 0;

	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	*pollfds = (struct pollfd *)malloc(servers.size() * sizeof(struct pollfd));

	for (server = servers.begin(); server != servers.end(); server++, i++)
	{
		server->createSocket();

		if (setsockopt(server->getSocketFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
			throw std::runtime_error(SETSOCKETOPT_ERR);

		if (getaddrinfo(server->getHost().c_str(), server->getPort().c_str(), &hints, &result) != 0)
			throw;

		if (bind(server->getSocketFd(), result->ai_addr, result->ai_addrlen) == -1)
			throw std::runtime_error("Bind exception");

		listen(server->getSocketFd(), NR_PENDING_CONNECTIONS);

		pollfds[0][i].fd = server->getSocketFd();
		pollfds[0][i].events = POLLIN;
		pollfds[0][i].revents = 0;

		freeaddrinfo(result);
		messageLog(server->getHost() + " : " + server->getPort(), BLUE, false);
	}

}

WebServ::~WebServ(void) {};
