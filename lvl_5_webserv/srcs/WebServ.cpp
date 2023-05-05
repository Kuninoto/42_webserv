#include "WebServ.hpp"

#include <memory.h>
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

bool g_stopServer = false;

static void closeServer(int signum) {
    (void)signum;
    messageLog("Closing server", YELLOW, false);
    g_stopServer = true;
}

WebServ::WebServ(std::string filename) {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, closeServer);

    this->servers = this->parseConfigFile(filename);
    this->duplicateServers();

    this->epollFd = epoll_create(MAX_EVENTS);
    if (this->epollFd == -1)
        throw std::runtime_error(EPOLL_CREATE_FAIL);

    this->reserved_sockets = this->open_fds = this->getServerUsedSockets();
};

void WebServ::bootServers(void) {
    int opt = 1;
    struct addrinfo hints, *result;
    struct epoll_event ev;
    ev.events = EPOLLIN;

    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::vector<Server>::iterator server;
    for (server = servers.begin(); server != servers.end(); server++) {
        if (server->getSkipBind()) continue;

        server->createSocket();

        if (setsockopt(server->getSocketFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
            throw std::runtime_error(SETSOCKETOPT_ERR);

        if (getaddrinfo(server->getHost().c_str(), server->getPort().c_str(), &hints, &result) != 0)
            throw std::runtime_error(GETADDRINFO_FAIL);

        if (bind(server->getSocketFd(), result->ai_addr, result->ai_addrlen) == -1)
            throw std::runtime_error(BIND_FAIL);

        listen(server->getSocketFd(), NR_PENDING_CONNECTIONS);

        ev.data.fd = server->getSocketFd();
        if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, server->getSocketFd(), &ev) == -1)
            throw std::runtime_error(EPOLL_CTL_FAIL);

        freeaddrinfo(result);
        messageLog(server->getHost() + ":" + server->getPort(), BLUE, false);
    }
}

void WebServ::runServers(void) {
    int nfds;
    struct epoll_event ev, events[MAX_EVENTS];

    while (true) {
        if ((nfds = epoll_wait(this->epollFd, events, MAX_EVENTS, -1)) == -1)
            throw std::runtime_error(EPOLL_WAIT_FAIL);

        if (g_stopServer)
            break;

        for (size_t i = 0; i < open_fds; i += 1) {
            char buffer[1024] = {0};
            if (events[i].data.fd < servers.at(0).getSocketFd())
                continue;

            if (events[i].events & EPOLLIN) {
                // if triggeredFdIsServer
                if (events[i].data.fd == servers.at(0).getSocketFd() || events[i].data.fd == servers.at(1).getSocketFd()) {
                    this->open_fds += 1;

                    int conn_socket = accept(servers.at(i).getSocketFd(), NULL, NULL);
                    if (conn_socket == -1)
                        throw std::runtime_error(ACCEPT_FAIL);

                    // setnonblocking(conn_sock);
                    ev.events = EPOLLIN | EPOLLOUT;
                    ev.data.fd = conn_socket;
                    if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, conn_socket, &ev) == -1)
                        throw std::runtime_error(EPOLL_CTL_FAIL);

                    recv(conn_socket, buffer, 1023, 0);
                    buffer[1023] = '\0';

                    clients.push_back(Client(getServerByName(buffer, servers.at(i)), conn_socket));
                    clients.back().setRequest(std::string(buffer));

                } else {
                    if (recv(events[i].data.fd, buffer, 1023, 0) > 0) {
                        clients.at(i - reserved_sockets).setRequest(std::string(buffer));
                    } else {
                        close(events[i].data.fd);
                        clients.erase(clients.begin() + i - reserved_sockets);
                        this->open_fds -= 1;
                    }
                }
            }

            if (events[i].data.fd < servers.at(reserved_sockets - 1).getSocketFd())
                continue;

            if (events[i].events & EPOLLOUT) {
                clients.at(i - reserved_sockets).response();
            }
        }
    }
    for (size_t i = 0; i < open_fds; i++) {
        close(events[i].data.fd);
        shutdown(events[i].data.fd, SHUT_RDWR);
    }
}

WebServ::~WebServ(void) {
    messageLog("Server closed", GREEN, false);
};

void WebServ::duplicateServers(void) {
    std::vector<Server>::iterator v_itr;
    std::vector<Server>::iterator v_itr2;

    for (v_itr = this->servers.begin(); v_itr != this->servers.end(); v_itr++) {
        if (v_itr->getServerName().length() == 0) continue;

        for (v_itr2 = v_itr + 1; v_itr2 != this->servers.end(); v_itr2++) {
            if (v_itr->getServerName() == v_itr2->getServerName())
                throw WebServ::ParserException("Duplicate servers");

            if (v_itr->getHost() == v_itr2->getHost() && v_itr->getPort() == v_itr2->getPort())
                v_itr2->setSkipBind();
        }
    }
}

size_t WebServ::getServerUsedSockets(void) {
    std::vector<Server>::iterator server;
    size_t used_sockets = 0;

    for (server = this->servers.begin(); server != this->servers.end(); server++) {
        if (!server->getSkipBind()) used_sockets += 1;
    }

    return used_sockets;
}

Server& WebServ::getServerByName(const std::string& buffer, Server& default_server) {
    std::vector<Server>::iterator server;
    std::string requested_server_name;

    if (size_t n = buffer.find("Host:")) {
        n += 6;
        requested_server_name = buffer.substr(n, (buffer.find("\n", n) - n - 1));
    }

    for (server = this->servers.begin(); server != this->servers.end(); server++) {
        if (requested_server_name == server->getServerName() && server->getHost() == default_server.getHost())
            return *server;
    }
    return default_server;
}

std::vector<Server> WebServ::parseConfigFile(std::string filename) {
    Lexer lexer(filename);
    std::vector<Server> servers;

    // Parse Server scopes
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
        if (curly_brackets > 0)
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
            throw WebServ::ParserException("invalid cgi_path");
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
