#include "libwebserv.hpp"

#include <iostream>
#include <stdio.h>

using std::cout;
using std::cerr;
using std::endl;

# define PORT 8080
# define NR_PENDING_CONNECTIONS 10

bool g_stopServer = false;

/* void printUintVecStorage(const std::vector<unsigned short>& v)
{
    std::vector<unsigned short>::const_iterator itr;

    for (itr = v.begin(); itr != v.end(); itr++)
        std::cout << "\"" << *itr << "\"";
    std::cout << std::endl;
} */

int main(int argc, char **argv)
{
	system("clear");

	if (argc != 2 || !argv[1][0])
		return panic(ARGS_ERR);

	Socket socket(PORT);
	if (!socket.setSocketFd())
		return panic(SOCKET_OPEN_ERR);

	WebServ server;

	try {
		server.parseConfigFile(argv[1]);
	}
	catch (std::exception& e) {
		cerr << ERROR_MSG_PREFFIX << "invalid config file: " << e.what() << endl;
		return EXIT_FAILURE;
	}

	// printUintVecStorage(server.getPorts());

	int opt = 1;
	if (setsockopt(socket.getSocketFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
		return panic(SETSOCKETOPT_ERR);

	struct sockaddr_in address;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;

	// How to handle more than one port ?
	address.sin_port = htons(server.getPorts().at(0));

	cout << "Trying to bind..." << endl;
	if (bind(socket.getSocketFd(), (struct sockaddr *)&address, sizeof(address)) == -1)
	{
		cout << errno << endl;
		return panic(BINDING_ERR);
	}
	cout << "Binding successful" << endl;

	//std::string server_name("olaamigos");
	//if (setsockopt(socket.getSocketFd(), SOL_SOCKET, SO_REUSEPORT, server_name.c_str(), server_name.length()) < 0)
	//	return panic(SETSOCKETOPT_ERR);

	std::vector<Client> clients;

	listen(socket.getSocketFd(), NR_PENDING_CONNECTIONS);

	int addrlen = sizeof(address);
	char buffer[1025] = {0};

	cout << "Listening..." << endl;

	size_t max_fds = 1;
	struct pollfd *pollfds = (struct pollfd *)malloc(max_fds * sizeof(struct pollfd));
	//struct pollfd *pollfds = new struct pollfd[max_fds];

	pollfds->fd = socket.getSocketFd();
	pollfds->events = POLLIN;
	size_t open_fds = 1;
	pollfds->revents = 0;
	size_t num;

	std::string buffer_cpp;

	cout << getTimeStamp() << endl;

	while (!g_stopServer)
	{
		num = open_fds;
		if (poll(pollfds, num, -1) == -1)
		{
			if (!g_stopServer)
				return panic(POLL_FAIL); // change this
		}

		for (size_t i = 0; i < open_fds; i += 1)
		{
			if (pollfds[i].fd <= 0)
				continue;

			if (pollfds[i].revents & POLLIN)
			{
				if (pollfds[i].fd == socket.getSocketFd())
				{
					if (open_fds == max_fds)
					{
						pollfds = (struct pollfd *)realloc(pollfds, sizeof(struct pollfd) * (open_fds + 1));
						max_fds += 1;
					}
					open_fds += 1;

					//TODO accept to inside constructor amd get it with getFd to save in pollfds[...].fd, also throw inside if error and catch here
					int temp = accept(socket.getSocketFd(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
					if (temp < 0)
						throw;
					pollfds[open_fds - 1].fd = temp; 
					clients.push_back(Client(server, temp));
					// -------

					pollfds[open_fds - 1].events = POLLIN | POLLOUT;
					pollfds[open_fds - 1].revents = 0;
					cout << "New request!" << endl;
				}
				else
				{
					if (recv(pollfds[i].fd, buffer, 1023, 0) >= 0)
					{
						buffer[1023] = '\0';
						clients.at(i - 1).setRequest(std::string(buffer));
						bzero(buffer, 1024);
					}
					else
					{
						cout << "connection close" << endl;
						close(pollfds[i].fd);
						for (size_t j = i; j < open_fds - 1; j++)
							pollfds[j].fd = pollfds[j + 1].fd;						
						pollfds[open_fds - 1].fd = -1;
						open_fds -= 1;
					}
				}
			}

			if (pollfds[i].revents & POLLOUT) {
				clients.at(i - 1).response();
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
