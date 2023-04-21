#include "libwebserv.hpp"

using std::cout;
using std::cerr;
using std::endl;

bool g_stopServer = false;

int main(int argc, char **argv)
{
	system("clear");

	if (argc != 2 || !argv[1][0])
		return panic(ARGS_ERR);

	WebServ webserv;

	std::vector<Server> servers;
	std::vector<Client> clients;
	struct pollfd *pollfds = NULL;

	//TODO Parse the config file
	try {
		servers = parseConfigFile(argv[1]);
	}
	catch (std::exception& e) {
		cerr << ERROR_MSG_PREFFIX << "invalid config file: " << e.what() << endl;
		return EXIT_FAILURE;
	}

	try
	{
		webserv.bootServers(servers, &pollfds);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	

/* 	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);
	cout << "Hostname: " << hostname << endl; */

	//std::string server_name("olaamigos");
	//if (setsockopt(socket.getSocketFd(), SOL_SOCKET, SO_REUSEPORT, server_name.c_str(), server_name.length()) < 0)
	//	return panic(SETSOCKETOPT_ERR);

	char buffer[1024];
	size_t reserved_socket_fd = 2, max_fds = servers.size(), open_fds = max_fds;
	size_t num;

	cout << getTimeStamp() << endl;

	while (!g_stopServer)
	{
		num = open_fds;
		if (poll(pollfds, num, -1) == -1)
		{
			if (!g_stopServer)
				return panic(POLL_FAIL); // change this
		}

		for (size_t i = 0; i < open_fds; i ++)
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
						throw;
					pollfds[open_fds - 1].fd = temp; 
					clients.push_back(Client(servers.at(i), temp));

					pollfds[open_fds - 1].events = POLLIN | POLLOUT;
					pollfds[open_fds - 1].revents = 0;
					cout << "New request!" << endl;
				}
				else
				{
					if (recv(pollfds[i].fd, buffer, 1023, 0) > 0)
					{
						buffer[1023] = '\0';
						clients.at(i - reserved_socket_fd).setRequest(std::string(buffer));
						bzero(buffer, 1024);
					}
					else
					{
						close(pollfds[i].fd);
						clients.erase(clients.begin() + i - reserved_socket_fd);
						for (size_t j = i; j < open_fds - 1; j++)
							pollfds[j].fd = pollfds[j + 1].fd;						
						pollfds[open_fds - 1].fd = -1;
						open_fds -= 1;
					}
				}
			}

			if (pollfds[i].fd < servers.at(reserved_socket_fd - 1).getSocketFd())
				continue;

			if (pollfds[i].revents & POLLOUT) {
				clients.at(i - reserved_socket_fd).response();
			}
		}
	}
	
	for (size_t i = 0; i < open_fds; i++)
	{
		close(pollfds[i].fd);
		shutdown(pollfds[i].fd, 0);
	}
	free(pollfds);
	messageLog("Server closed", GREEN, false);
	return EXIT_SUCCESS;
}
