#include "WebServ.hpp"

# include <stdlib.h>
# include <signal.h>
# include <string.h>
# include <netdb.h>
# include <sys/types.h>

# define NR_PENDING_CONNECTIONS 10

bool g_stopServer = false;

static void closeServer(int signum)
{
	(void) signum;
	messageLog("Closing server", YELLOW, false);
	g_stopServer = true;
}

WebServ::WebServ(std::string filename)
{
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, closeServer);

	servers = parseConfigFile(filename);
	reserved_sockets
		= max_fds
		= open_fds 
		= servers.size();
};

void WebServ::bootServers()
{
	std::vector<Server>::iterator server;
	struct addrinfo hints, *result;
	int opt = 1;
	int i = 0;

	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	pollfds = (struct pollfd *)malloc(servers.size() * sizeof(struct pollfd));

	for (server = servers.begin(); server != servers.end(); server++, i++)
	{
		server->createSocket();

		if (setsockopt(server->getSocketFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
			throw std::runtime_error(SETSOCKETOPT_ERR);

		if (getaddrinfo(server->getHost().c_str(), server->getPort().c_str(), &hints, &result) != 0)
			throw std::runtime_error("getaddrinfo() failed");

		if (bind(server->getSocketFd(), result->ai_addr, result->ai_addrlen) == -1)
			throw std::runtime_error("bind() failed");

		listen(server->getSocketFd(), NR_PENDING_CONNECTIONS);

		pollfds[i].fd = server->getSocketFd();
		pollfds[i].events = POLLIN;
		pollfds[i].revents = 0;

		freeaddrinfo(result);
		messageLog(server->getHost() + " : " + server->getPort(), BLUE, false);
	}
}

void WebServ::runServers()
{
	char buffer[1024];
	size_t num;

	while (!g_stopServer)
	{
		num = open_fds;
		if (poll(pollfds, num, -1) == -1)
		{
			if (!g_stopServer)
				throw std::runtime_error(POLL_FAIL); // change this
		}

		for (size_t i = 0; i < open_fds; i += 1)
		{
			if (pollfds[i].fd < servers.at(0).getSocketFd())
				continue;

			if (pollfds[i].revents & POLLIN)
			{
				if (pollfds[i].fd == servers.at(0).getSocketFd() || pollfds[i].fd == servers.at(1).getSocketFd())
				{
					if (open_fds == max_fds)
					{
						pollfds = (struct pollfd *)realloc(pollfds, sizeof(struct pollfd) * (open_fds + 1));
						max_fds += 1;
					}
					open_fds += 1;

					int temp = accept(servers.at(i).getSocketFd(), NULL, NULL);
					if (temp < 0)
						throw std::runtime_error("failed to accept connection");
					pollfds[open_fds - 1].fd = temp; 
					clients.push_back(Client(servers.at(i), temp));

					pollfds[open_fds - 1].events = POLLIN | POLLOUT;
					pollfds[open_fds - 1].revents = 0;
					std::cout << "New request!" << std::endl;
				}
				else
				{
					if (recv(pollfds[i].fd, buffer, 1023, 0) > 0)
					{
						buffer[1023] = '\0';
						clients.at(i - reserved_sockets).setRequest(std::string(buffer));
						bzero(buffer, 1024);
					}
					else
					{
						close(pollfds[i].fd);
						clients.erase(clients.begin() + i - reserved_sockets);
						for (size_t j = i; j < open_fds - 1; j++)
							pollfds[j].fd = pollfds[j + 1].fd;						
						pollfds[open_fds - 1].fd = -1;
						open_fds -= 1;
					}
				}
			}

			if (pollfds[i].fd < servers.at(reserved_sockets - 1).getSocketFd())
				continue;

			if (pollfds[i].revents & POLLOUT) {
				clients.at(i - reserved_sockets).response();
			}
		}
	}
}

WebServ::~WebServ(void) {

	for (size_t i = 0; i < open_fds; i++)
	{
		close(pollfds[i].fd);
		shutdown(pollfds[i].fd, 0);
	}

	free(pollfds);
	messageLog("Server closed", GREEN, false);
};
