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
    logMessage("Closing WebServer");
    g_stopServer = true;
}

WebServ::WebServ(std::string filename) {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, closeServer);

    this->servers = this->parseConfigFile(filename);
    this->duplicateServers();

    this->reserved_sockets = this->getServerUsedSockets();
};

bool WebServ::isFdAServer(int fd) {
    std::vector<Server>::iterator v_it;
    std::vector<Server>::iterator v_end = this->servers.end();

    for (v_it = this->servers.begin(); v_it != v_end; v_it++) {
        if (fd == v_it->getSocketFd())
            return true;
    }
    return false;
}

void WebServ::bootServers(void) {
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

        if (getaddrinfo(server->getHost().c_str(), server->getPort().c_str(), &hints, &result) != 0)
            throw std::runtime_error("fatal: getaddrinfo(): " + std::string(strerror(errno)));

        if (bind(server->getSocketFd(), result->ai_addr, result->ai_addrlen) == -1)
            throw std::runtime_error("fatal: bind(): \"" + std::string(strerror(errno)));

        if (listen(server->getSocketFd(), MAX_PENDING_CONNECTIONS) == -1)
            throw std::runtime_error("fatal: listen(): \"" + std::string(strerror(errno)));

        freeaddrinfo(result);
        pollstruct.fd = server->getSocketFd();
        pollstruct.events = POLLIN;
        pollstruct.revents = 0;
        pollfds.push_back(pollstruct);
        logMessage(server->getHost() + ":" + server->getPort());
    }
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
}

void WebServ::runServers(void) {
    while (!g_stopServer) {
        if (poll(this->pollfds.data(), this->pollfds.size(), 200) == -1 && !g_stopServer)
            throw std::runtime_error(POLL_FAIL);
        for (size_t i = 0; i < this->pollfds.size(); i += 1) {
            int clientFdIdx = i - reserved_sockets;

            if (pollfds.at(i).revents & POLLIN) {
                char buffer[1024] = {0};

                if (isFdAServer(this->pollfds.at(i).fd)) {
                    acceptClientConnection(i);
                } else {
                    if (recv(pollfds.at(i).fd, buffer, 1023, 0) > 0) {
                        clients.at(clientFdIdx).setRequest(std::string(buffer));
                    } else {
                        closeClientConnection(i, clientFdIdx);
                        continue;
                    }
                }
            }
            if (pollfds.at(i).revents & POLLERR) {
                logMessage(RED "POLLERR");
                closeClientConnection(i, clientFdIdx);
                continue;
            }
            if (pollfds.at(i).revents & POLLHUP) {
                logMessage(RED "POLLHUP");
                closeClientConnection(i, clientFdIdx);
                continue;
            }
            if (pollfds.at(i).revents & POLLNVAL) {
                logMessage(RED "POLLNVAL");
                closeClientConnection(i, clientFdIdx);
                continue;
            }

            if (this->pollfds.at(i).fd < servers.at(reserved_sockets - 1).getSocketFd())
                continue;

            if (this->pollfds.at(i).revents & POLLOUT) {
                if (!clients.at(clientFdIdx).preparedToSend())
                    continue;
                // REFACTOR THIS LINE
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
    logMessage("WebServer closed");
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
        if (server->isDefaultServer)
            nr_sockets += 1;
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
        if (requested_server_name == server->getServerName() && server->getHost() == default_server.getHost())
            return *server;
    }
    return default_server;
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
                if (token.type != LEFT_CURLY_BRACKET)
                    throw WebServ::ParserException("no opening bracket for server");
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
                servers.back().locations.insert(parseLocation(lexer.parameters, lexer.parameters["location"]));
                lexer.parameters.clear();
            } else if (curly_brackets == 0 && !hasLocation) {
                servers.push_back(Server(lexer.parameters));
                break;
            } else if (curly_brackets == 0) {
                break;
            }
            token = lexer.nextToken();
        }

        if (curly_brackets != 0)
            throw WebServ::ParserException("Unclosed curly brackets");
        token = lexer.nextToken();
    }

    return servers;
}

locationPair WebServ::parseLocation(const std::map<std::string, std::string>& lexerParameters,
                                    std::string& locationPath) {
    location_t locationStruct;
    bzero(&locationStruct, sizeof(location_t));

    if (lexerParameters.count("root") > 0)
        locationStruct.root = lexerParameters.find("root")->second;
    if (lexerParameters.count("allow_methods") > 0)
        locationStruct.allowed_methods = splitStr(lexerParameters.find("allow_methods")->second, ' ');
    if (lexerParameters.count("return") > 0)
        locationStruct.redirect = lexerParameters.find("return")->second;
    if (lexerParameters.count("auto_index") > 0)
        locationStruct.auto_index = lexerParameters.find("auto_index")->second == "on" ? true : false;
    if (lexerParameters.count("try_file") > 0)
        locationStruct.try_file = lexerParameters.find("try_file")->second;
    if (lexerParameters.count("cgi_path") > 0) {
        std::string temp = lexerParameters.find("cgi_path")->second;
        if (access(temp.c_str(), X_OK) != 0) {
            throw WebServ::ParserException("invalid cgi_path \"" + temp + "\"");
        }
        locationStruct.cgi_path = temp;
    }
    if (lexerParameters.count("cgi_ext") > 0)
        locationStruct.cgi_ext = lexerParameters.find("cgi_ext")->second;
    trimStr(locationPath, " ");
    return std::make_pair<std::string, location_t>(locationPath, locationStruct);
}

void WebServ::readLocationBlock(Lexer& lexer, Token& token) {
    token = lexer.nextToken();
    while (token.type != RIGHT_CURLY_BRACKET) {
        token = lexer.nextToken();
    }
}
