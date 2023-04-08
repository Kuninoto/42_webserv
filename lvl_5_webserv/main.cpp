#include "libwebserv.hpp"

#include <stdio.h>
#include <iostream>
#include <fstream>

using std::cout;
using std::endl;

# define NR_PENDING_CONNECTIONS 10

int main(int argc, char **argv)
{
	if (argc != 2 || !argv[1][0])
		return panic(ARGS_ERR);

	Socket socket(8080);
	if (!socket.setSocketFd())
		return panic(SOCKET_OPEN_ERR);

	WebServ server = WebServ(8080);

	cout << "WEB SERVER" << endl;
	cout << "Port: " << server.getPort() << endl;

	int opt;
	if (setsockopt(socket.getSocketFd(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(int)) < 0)
		return panic(SETSOCKETOPT_ERR);


	struct sockaddr_in address;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	cout << "Port" << endl;
	address.sin_port = htons(server.getPort());

	cout << "Trying to bind..." << endl;
	if (bind(socket.getSocketFd(), (struct sockaddr *)&address, sizeof(address)) == -1)
		return panic(BINDING_ERR);
	cout << "Binding successful" << endl;

	listen(socket.getSocketFd(), NR_PENDING_CONNECTIONS);

	int addrlen = sizeof(address);
	int new_socket;
	char buffer[1025] = {0};

	cout << "Listening..." << endl;

	struct pollfd *pollfds = new struct pollfd[10];

	pollfds->fd = socket.getSocketFd();
	pollfds->events = POLLIN;
	pollfds->revents = 0;
	size_t numfds = 1;
	size_t num;

	std::string buffer_cpp;

	int new_fd;

	while (true)
	{
		num = numfds;
		if (poll(pollfds, num, -1) == -1)
		{
			delete[] pollfds;
			return panic(POLL_ERR);
		}

		for (size_t i = 0; i <= numfds; i += 1)
		{
			if (pollfds[0].fd <= 0)
				continue;

			if (pollfds[0].revents & POLLIN) // pollfds[i].fd is not the one ready to read
			{
				if (pollfds[i].fd == socket.getSocketFd())
				{
					new_fd = accept(socket.getSocketFd(), (struct sockaddr *)&address, (socklen_t *)&addrlen); // == -1 -> erro
					cout << "New Request" << endl << endl;

					while (true)
					{
						recv(new_fd, buffer, 1024, 0);
						buffer_cpp.append(buffer);
						if (buffer_cpp.find("\r\n\r\n") != std::string::npos)
						{
							cout << buffer_cpp << endl;
							buffer_cpp.clear();
							break;
						}
						bzero(buffer, 1024);
					}

					// std::ifstream file("index.php");

					// if (!file.is_open())
					// 	std::cerr << "Failed to open file" << '\n';

					// std::string hello((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

					std::ifstream img_file("folhas.png", std::ios::binary | std::ios::in);

					std::string header("HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length: 85405\r\n\r\n");
					std::string img((std::istreambuf_iterator<char>(img_file)), std::istreambuf_iterator<char>());

					write(new_fd , header.c_str() , header.length());
					write(new_fd , img.c_str(), img.length());
					close(new_fd);
				}
			}
		}
	}

	delete[] pollfds;
	close(new_socket);
	shutdown(socket.getSocketFd(), SHUT_RDWR);
	close(socket.getSocketFd());
	return EXIT_SUCCESS;
}
