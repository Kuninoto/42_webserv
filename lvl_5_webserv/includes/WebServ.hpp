#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <stdint.h>
# include <string>

class WebServ {
	public:
		WebServ(uint16_t port);
		~WebServ(void);

		uint16_t getPort(void) const;

	private:
		const uint32_t port;

		WebServ(void);
};

#endif // WEBSERV_HPP