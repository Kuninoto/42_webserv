#include "libwebserv.hpp"

using std::cout;
using std::cerr;
using std::endl;

# define NR_PENDING_CONNECTIONS 10

bool g_stopServer = false;

// void configureServers(std::vector<Server> &servers, struct pollfd **pollfds)
// {
// 	std::vector<Server>::iterator server;
// 	struct addrinfo hints, *result;
// 	int opt = 1;
// 	int i = 0;

// 	bzero(&hints, sizeof(hints));
// 	hints.ai_family = AF_INET;
// 	hints.ai_socktype = SOCK_STREAM;
// 	hints.ai_protocol = IPPROTO_TCP;

// 	*pollfds = (struct pollfd *)malloc(servers.size() * sizeof(struct pollfd));

// 	for (server = servers.begin(); server != servers.end(); server++, i++)
// 	{
// 		server->createSocket();

// 		if (setsockopt(server->getSocketFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
// 			throw std::runtime_error(SETSOCKETOPT_ERR);

// 		cout << server->getHost().c_str() <<  server->getPort().c_str() << endl;

// 		if (getaddrinfo(server->getHost().c_str(), server->getPort().c_str(), &hints, &result) != 0)
// 			throw;

// 		cout << "Trying to bind..." << endl;
// 		if (bind(server->getSocketFd(), result->ai_addr, result->ai_addrlen) == -1)
// 			throw;
// 		cout << "Binding successful" << endl;

// 		cout << "Listening..." << endl;
// 		listen(server->getSocketFd(), NR_PENDING_CONNECTIONS);

// 		pollfds[0][i].fd = server->getSocketFd();
// 		pollfds[0][i].events = POLLIN;
// 		pollfds[0][i].revents = 0;

// 		freeaddrinfo(result);
// 	}
// }

// int main(int argc, char **argv)
// {
// 	system("clear");
// 	if (argc != 2 || !argv[1][0])
// 		return panic(ARGS_ERR);

// 	std::vector<Server> servers;
// 	std::vector<Client> clients;
// 	struct pollfd *pollfds = NULL;

// 	try {
// 		servers = parseConfigFile(argv[1]);
// 		configureServers(servers, &pollfds);
// 	}
// 	catch (std::exception& e) {
// 		cerr << ERROR_MSG_PREFFIX << "fatal: " << e.what() << endl;
// 		return EXIT_FAILURE;
// 	}

// 	char hostname[1024];
// 	hostname[1023] = '\0';
// 	gethostname(hostname, 1023);
// 	cout << "Hostname: " << hostname << endl;

// 	//std::string server_name("olaamigos");
// 	//if (setsockopt(socket.getSocketFd(), SOL_SOCKET, SO_REUSEPORT, server_name.c_str(), server_name.length()) < 0)
// 	//	return panic(SETSOCKETOPT_ERR);

// 	char buffer[1024];
// 	size_t reserved_socket_fd = 2, max_fds = servers.size(), open_fds = max_fds;
// 	size_t num;

// 	cout << getTimeStamp() << endl;

// 	while (!g_stopServer)
// 	{
// 		num = open_fds;
// 		if (poll(pollfds, num, -1) == -1)
// 		{
// 			if (!g_stopServer)
// 				return panic(POLL_FAIL); // change this
// 		}

// 		for (size_t i = 0; i < open_fds; i += 1)
// 		{
// 			if (pollfds[i].fd < servers.at(0).getSocketFd())
// 				continue;

// 			if (pollfds[i].revents & POLLIN)
// 			{
// 				if (pollfds[i].fd == servers.at(0).getSocketFd() || pollfds[i].fd == servers.at(1).getSocketFd())
// 				{
// 					if (open_fds == max_fds)
// 					{
// 						pollfds = (struct pollfd *)realloc(pollfds, sizeof(struct pollfd) * (open_fds + 1));
// 						max_fds += 1;
// 					}
// 					open_fds += 1;

// 					//TODO accept to inside constructor and get it with getFd to save in pollfds[...].fd, also throw inside if error and catch here
// 					int temp = accept(servers.at(i).getSocketFd(), NULL, NULL);
// 					if (temp < 0)
// 						throw;
// 					pollfds[open_fds - 1].fd = temp; 
// 					clients.push_back(Client(servers.at(i), temp));
// 					// -------

// 					pollfds[open_fds - 1].events = POLLIN | POLLOUT;
// 					pollfds[open_fds - 1].revents = 0;
// 					cout << "New request!" << endl;
// 				}
// 				else
// 				{
// 					if (recv(pollfds[i].fd, buffer, 1023, 0) > 0)
// 					{
// 						buffer[1023] = '\0';
// 						clients.at(i - reserved_socket_fd).setRequest(std::string(buffer));
// 						bzero(buffer, 1024);
// 					}
// 					else
// 					{
// 						cout << "connection close" << endl;
// 						cout << i << endl;
// 						close(pollfds[i].fd);
// 						for (size_t j = i; j < open_fds - 1; j++)
// 							pollfds[j].fd = pollfds[j + 1].fd;						
// 						pollfds[open_fds - 1].fd = -1;
// 						open_fds -= 1;
// 					}
// 				}
// 			}

// 			if (pollfds[i].revents & POLLOUT) {
// 				clients.at(i - reserved_socket_fd).response();
// 			}
// 		}
// 	}
	
// 	for (size_t i = 0; i < open_fds; i++)
// 	{
// 		close(pollfds[i].fd);
// 		shutdown(pollfds[i].fd, 0);
// 	}
// 	free(pollfds);
// 	messageLog("Server closed", GREEN, false);

// 	return EXIT_SUCCESS;
// }


int main()
{
	try
	{
		std::string tmp("POST /scripts/test.pyy HTTP/1.1\nHost: www.example.com\nContent-Type: application/x-www-form-urlencoded\nContent-Length: 27\n\nname=John+Doe&age=30&gender=M\n");

		cout << "--------Debug--------" << endl;
		cout << "---------------------" << endl;
		setenv("PATH_INFO", "/home/Flirt/Desktop/Projects_42/Webserver/lvl_5_webserv", 0);

		CGI cgi(tmp);
		cout << "Expected output: \"Hello bitches!!!!!!!!\"" << endl;
		cout << "Script output  : \"" << cgi.response << "\"" << endl;
		cout << "---------------------" << endl;
		cout << "------Finished-------" << endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

}