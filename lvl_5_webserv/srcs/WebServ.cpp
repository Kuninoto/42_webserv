#include "WebServ.hpp"

WebServ::WebServ(uint16_t port) : port(port) {};

WebServ::~WebServ(void){};

uint16_t WebServ::getPort(void) const {
	return this->port;
}
