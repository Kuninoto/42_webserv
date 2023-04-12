#include "libwebserv.hpp"

#include <stdio.h>
#include <iostream>
#include <fstream>

using std::cout;
using std::endl;

# define PORT 8080
# define NR_PENDING_CONNECTIONS 10

bool g_stopServer = false;

int main(int argc, char **argv)
{
	if (argc != 2 || !argv[1][0])
		return panic(ARGS_ERR);

	Socket socket(PORT);
	if (!socket.setSocketFd())
		return panic(SOCKET_OPEN_ERR);

	WebServ server = WebServ(PORT);

	server.parseConfigFile(argv[1]);

	cout << "WEB SERVER" << endl;
	cout << "Port: " << server.getPort() << endl;

	int opt = 0;
	if (setsockopt(socket.getSocketFd(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(int)) < 0)
		return panic(SETSOCKETOPT_ERR);

	struct sockaddr_in address;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(server.getPort());

	cout << "Trying to bind..." << endl;
	if (bind(socket.getSocketFd(), (struct sockaddr *)&address, sizeof(address)) == -1)
		return panic(BINDING_ERR);
	cout << "Binding successful" << endl;

	//std::string server_name("olaamigos");
	//if (setsockopt(socket.getSocketFd(), SOL_SOCKET, SO_REUSEPORT, server_name.c_str(), server_name.length()) < 0)
	//	return panic(SETSOCKETOPT_ERR);

	listen(socket.getSocketFd(), NR_PENDING_CONNECTIONS);

	int addrlen = sizeof(address);
	char buffer[1025] = {0};

	cout << "Listening..." << endl;

	struct pollfd *pollfds = new struct pollfd[10];

	pollfds->fd = socket.getSocketFd();
	pollfds->events = POLLIN;
	pollfds->revents = 0;
	size_t numfds = 1;
	size_t num;

	std::string buffer_cpp;

	cout << getTimeStamp() << endl;

	while (!g_stopServer)
	{
		num = numfds;
		if (poll(pollfds, num, -1) == -1)
		{
			delete[] pollfds;
			if (!g_stopServer)
				return panic(POLL_FAIL);
			else
				return 0;
		}

		for (size_t i = 0; i <= numfds; i += 1)
		{
			if (pollfds[i].fd <= 0)
				continue;

			if (pollfds[i].revents & POLLIN)
			{
				if (pollfds[i].fd == socket.getSocketFd())
				{
					numfds += 1;
					pollfds[numfds - 1].fd = accept(socket.getSocketFd(), (struct sockaddr *)&address, (socklen_t *)&addrlen); // == -1 -> erro
					pollfds[numfds - 1].events = POLLIN | POLLOUT;
					pollfds[numfds - 1].revents = 0;
					cout << "New request!" << endl;
				}
				else
				{
					if (recv(pollfds[i].fd, buffer, 1024, 0) > 0)
					{					
						cout << endl;
						cout << buffer << endl;
						bzero(buffer, 1024);
					}
					else
					{
						cout << "connection close" << endl;
						pollfds[i].fd = -1;
						//close(pollfds[i].fd);
						break;
					}
				}
			}

			if (pollfds[i].revents & POLLOUT)
			{
				std::ifstream file("pages/error/404.html");

				if (!file.is_open())
					std::cerr << "Failed to open file" << '\n';

				std::string hello((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());	

				write(pollfds[numfds - 1].fd , hello.c_str() , hello.length());


				/* std::ifstream img_file("folhas.png", std::ios::binary | std::ios::in);

				std::string header("HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length: 85405\r\n\r\n");
				std::string img((std::istreambuf_iterator<char>(img_file)), std::istreambuf_iterator<char>()); */

				/* write(pollfds[numfds - 1].fd , header.c_str() , header.length());
				write(pollfds[numfds - 1].fd , img.c_str(), img.length()); */
			}
		}
	}

	delete[] pollfds;
	shutdown(socket.getSocketFd(), SHUT_RDWR);
	close(socket.getSocketFd());
	return EXIT_SUCCESS;
}
