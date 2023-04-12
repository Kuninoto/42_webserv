#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <stdint.h>
# include <string>
# include <map>

class WebServ {
	public:
		WebServ(uint16_t port);
		~WebServ(void);

		uint16_t getPort(void) const;

		void parseConfigFile(std::string file);

	private:
		const uint32_t port;
		const std::map<int, std::string> error_messages;

		WebServ(void);
};

#endif // WEBSERV_HPP