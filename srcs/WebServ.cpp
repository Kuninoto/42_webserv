#include "WebServ.hpp"

#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

bool g_stopServer = false;

static void closeServer(int signum) {
    (void)signum;
    std::cout << "\n";
    logMessage("Closing 42_Webserv...");
    g_stopServer = true;
}

WebServ::WebServ(const std::string& configFile) {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, closeServer);

    this->servers = this->parseConfigFile(configFile);
    this->duplicateServers();

    this->reserved_sockets = this->getServerUsedSockets();
};

void WebServ::bootServers(void) {
    logMessage("Booting servers...");
    int opt = 1;
    struct addrinfo hints, *result;

    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::vector<Server>::iterator server;
    for (server = servers.begin(); server != servers.end(); server++) {
        if (!server->isDefaultServer)
            continue;
        struct pollfd pollstruct;

        server->createSocket();

        if (setsockopt(server->getSocketFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
            throw std::runtime_error("fatal: setsocketopt(): " + std::string(strerror(errno)));

        if (getaddrinfo(server->getHost().c_str(), server->getPort().c_str(), &hints, &result) == -1) {
            freeaddrinfo(result);
            result = NULL;
            throw std::runtime_error("fatal: getaddrinfo(): " + std::string(strerror(errno)));
        }

        if (result) {
            if (bind(server->getSocketFd(), result->ai_addr, result->ai_addrlen) == -1) {
                throw std::runtime_error("fatal: bind(): " + std::string(strerror(errno)));
            }
        }

        if (listen(server->getSocketFd(), MAX_PENDING_CONNECTIONS) == -1)
            throw std::runtime_error("fatal: listen(): " + std::string(strerror(errno)));

        freeaddrinfo(result);
        pollstruct.fd = server->getSocketFd();
        pollstruct.events = POLLIN;
        pollstruct.revents = 0;
        pollfds.push_back(pollstruct);
        logMessage(server->getHost() + ":" + server->getPort());
    }
}

bool WebServ::isFdAServer(int fd) {
    std::vector<Server>::iterator v_it;
    std::vector<Server>::iterator v_end = this->servers.end();

    for (v_it = this->servers.begin(); v_it != v_end; v_it++) {
        if (v_it->isDefaultServer && fd == v_it->getSocketFd())
            return true;
    }
    return false;
}

void WebServ::acceptClientConnection(int idx) {
    int conn_socket = accept(servers.at(idx).getSocketFd(), NULL, NULL);
    if (conn_socket == -1)
        throw std::runtime_error(ACCEPT_FAIL);

    fcntl(conn_socket, F_SETFL, O_NONBLOCK);

    clients.push_back(Client(servers.at(idx), conn_socket));

    struct pollfd pollstruct;
    pollstruct.fd = conn_socket;
    pollstruct.events = POLLIN | POLLOUT;
    pollstruct.revents = 0;
    this->pollfds.push_back(pollstruct);
}

void WebServ::closeClientConnection(int idx, int clientFdIdx) {
    close(pollfds.at(idx).fd);
    pollfds.erase(pollfds.begin() + idx);
    clients.erase(clients.begin() + clientFdIdx);
    logMessage("Connection closed");
}

void WebServ::runServers(void) {
    while (!g_stopServer) {
        if (poll(this->pollfds.data(), this->pollfds.size(), 200) == -1 && !g_stopServer)
            throw std::runtime_error(POLL_FAIL);
        for (size_t i = 0; i < this->pollfds.size(); i += 1) {
            int clientFdIdx = i - reserved_sockets;
            short revent = pollfds.at(i).revents;

            if (revent & POLLIN) {
                if (isFdAServer(this->pollfds.at(i).fd)) {
                    acceptClientConnection(i);
                    continue;
                }

                char buffer[2048] = {0};
                int rd_bytes;
                if ((rd_bytes = recv(pollfds.at(i).fd, buffer, 2048, 0)) > 0) {
                    clients.at(clientFdIdx).setRequest(buffer, rd_bytes);
                } else {
                    closeClientConnection(i, clientFdIdx);
                }
                continue;
            }

            if (revent & POLLERR) {
                logMessage("Connection closed" RED "ERROR: " RESET "POLLERR");
                closeClientConnection(i, clientFdIdx);
                continue;
            }

            if (revent & POLLHUP) {
                logMessage("Connection closed" RED "ERROR: " RESET "POLLHUP");
                closeClientConnection(i, clientFdIdx);
                continue;
            }

            if (revent & POLLNVAL) {
                logMessage("Connection closed" RED "ERROR: " RESET "POLLNVAL");
                closeClientConnection(i, clientFdIdx);
                continue;
            }

            if (this->pollfds.at(i).fd < servers.at(reserved_sockets - 1).getSocketFd())
                continue;

            if (revent & POLLOUT) {
                if (clients.at(clientFdIdx).timeout()) {
                    logMessage("Time out");
                    closeClientConnection(i, clientFdIdx);
                    continue;
                }
                if (!clients.at(clientFdIdx).preparedToSend())
                    continue;
                clients.at(clientFdIdx).setTargetServer(getServerByName(clients.at(clientFdIdx).getRequest(), clients.at(clientFdIdx).getTargetServer()));
                clients.at(clientFdIdx).response();
            }
        }
    }
}

WebServ::~WebServ(void) {
    for (size_t i = 0; i < pollfds.size(); i += 1) {
        shutdown(pollfds[i].fd, SHUT_RDWR);
        close(pollfds[i].fd);
    }
    logMessage("Closed successfully!");
};

void WebServ::duplicateServers(void) {
    std::vector<Server>::iterator v_itr;
    std::vector<Server>::iterator v_itr2;

    for (v_itr = this->servers.begin(); v_itr != this->servers.end(); v_itr++) {
        if (v_itr->getServerName().length() == 0)
            continue;

        for (v_itr2 = v_itr + 1; v_itr2 != this->servers.end(); v_itr2++) {
            if (v_itr->getServerName() == v_itr2->getServerName())
                throw WebServ::ParserException("Duplicate servers");

            if (v_itr->getHost() == v_itr2->getHost() && v_itr->getPort() == v_itr2->getPort())
                v_itr2->isDefaultServer = false;
        }
    }
}

size_t WebServ::getServerUsedSockets(void) {
    std::vector<Server>::iterator server;
    size_t nr_sockets = 0;

    for (server = this->servers.begin(); server != this->servers.end(); server++) {
        if (server->isDefaultServer) {
            nr_sockets += 1;
        }
    }
    return nr_sockets;
}

Server& WebServ::getServerByName(const std::string& buffer, Server& default_server) {
    std::string requested_server_name;

    if (size_t n = buffer.find("Host:")) {
        n += 6;
        requested_server_name = buffer.substr(n, (buffer.find("\n", n) - n - 1));
    }

    std::vector<Server>::iterator server;
    for (server = this->servers.begin(); server != this->servers.end(); server++) {
        if (requested_server_name == server->getServerName() && server->getHost() == default_server.getHost()) {
            return *server;
        }
    }
    return default_server;
}

static locationPair parseLocation(const std::map<std::string, std::string>& lexerParameters,
                                  std::string& locationPath) {
    location_t newLocation;

    if (lexerParameters.count("root") > 0) {
        std::string tempRoot = lexerParameters.find("root")->second;
        if (*(tempRoot.end() - 1) != '/')
            newLocation.root = tempRoot + "/";
        else
            newLocation.root = tempRoot;
    }
    if (lexerParameters.count("allow_methods") > 0)
        newLocation.allowed_methods = splitStr(lexerParameters.find("allow_methods")->second, ' ');
    if (lexerParameters.count("return") > 0)
        newLocation.redirect = lexerParameters.find("return")->second;
    if (lexerParameters.count("auto_index") > 0)
        newLocation.auto_index = lexerParameters.find("auto_index")->second == "on" ? true : false;
    else
        newLocation.auto_index = false;
    if (lexerParameters.count("try_file") > 0)
        newLocation.try_file = lexerParameters.find("try_file")->second;
    if (lexerParameters.count("cgi_path") > 0 && lexerParameters.count("cgi_ext") > 0) {
        newLocation.hasCGI = true;

        std::string temp_path = lexerParameters.find("cgi_path")->second;
        if (*(temp_path.end() - 1) != '/')
            newLocation.cgi_path = temp_path + "/";
        else
            newLocation.cgi_path = temp_path;

        newLocation.cgi_ext = lexerParameters.find("cgi_ext")->second;
    } else
        newLocation.hasCGI = false;
    if (lexerParameters.count("upload_to") > 0) {
        std::string tempUploadTo = lexerParameters.find("upload_to")->second;

        if (tempUploadTo.at(0) == '/')
            newLocation.uploadTo = tempUploadTo.erase(0, 1);
        else
            newLocation.uploadTo = tempUploadTo;
    }
    trimStr(locationPath, " ");
    return std::make_pair<std::string, location_t>(locationPath, newLocation);
}

static void readLocationBlock(Lexer& lexer, Token& token) {
    token = lexer.nextToken();
    while (token.type != RIGHT_CURLY_BRACKET) {
        token = lexer.nextToken();
    }
}

std::vector<Server> WebServ::parseConfigFile(const std::string& filename) {
    Lexer lexer(filename);
    std::vector<Server> servers;

    Token token = lexer.nextToken();
    while (token.type != END_OF_FILE) {
        bool hasLocation = false;
        int curly_brackets = 0;
        while (true) {
            if (token.value == "server") {
                token = lexer.nextToken();
            }

            if (token.type == LEFT_CURLY_BRACKET) {
                curly_brackets += 1;
            } else if (token.type == RIGHT_CURLY_BRACKET) {
                curly_brackets -= 1;
            }
            if (token.value == "location") {
                if (hasLocation == false) {
                    hasLocation = true;
                    servers.push_back(Server(lexer.parameters));
                    std::string temp = lexer.parameters["location"];
                    lexer.parameters.clear();
                    lexer.parameters["location"] = temp;
                }
                readLocationBlock(lexer, token);
                servers.back().addLocation(parseLocation(lexer.parameters, lexer.parameters["location"]));
                lexer.parameters.clear();
            } else if (curly_brackets == 0 && !hasLocation) {
                servers.push_back(Server(lexer.parameters));
                break;
            } else if (curly_brackets == 0) {
                break;
            }
            token = lexer.nextToken();
        }
        token = lexer.nextToken();
    }
    return servers;
}
