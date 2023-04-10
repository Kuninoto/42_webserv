#include "WebServ.hpp"

#include <signal.h>
#include <iostream>

extern bool g_stopServer;

static void closeServer(int signum) {
	(void) signum;
	std::cout << "\n\nClosing server..." << std::endl;
	g_stopServer = true;
}

WebServ::WebServ(uint16_t port) : port(port) {
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, closeServer);
};

WebServ::~WebServ(void){};

uint16_t WebServ::getPort(void) const {
	return this->port;
}
