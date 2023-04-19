#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include "WebServ.hpp"

extern bool g_stopServer;

static void closeServer(int signum)
{
	(void) signum;
	messageLog("Closing server", YELLOW, false);
	g_stopServer = true;
}

WebServ::WebServ(void)
{
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, closeServer);
};

WebServ::~WebServ(void) {};
