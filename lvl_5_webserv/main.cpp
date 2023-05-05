#include "libwebserv.hpp"

using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char **argv)
{
	system("clear");
	if (argc != 2 || !argv[1][0])
		return panic(ARGS_ERR);

	try {
		cout << getTimeStamp() << endl;
		WebServ webserv(argv[1]);
		messageLog("Booting servers...", RESET, false);
		webserv.bootServers();
		webserv.runServers();
		return EXIT_SUCCESS;
	}
	catch (std::exception& e) {
		cerr << ERROR_MSG_PREFFIX << e.what() << '\n';
		return EXIT_FAILURE;
	}
}

/* int main(void)
{
	try
	{
		std::string request_example("POST scripts/test.py HTTP/1.1\nHost: www.example.com\nContent-Type: application/x-www-form-urlencoded\nContent-Length: 27\n\nname=John+Doe&age=30&gender=M\n");


		cout << "--------Debug--------" << endl;
		cout << "---------------------" << endl;
		setenv("PATH_INFO", "/nfs/homes/roramos/Documents/42_webserv/lvl_5_webserv", 0);

		CGI cgi(request_example);
		cout << "Expected output: \"Hello from test.py!\"" << endl;
		cout << "Script output  : \"" << cgi.response << "\"" << endl;
		cout << "---------------------" << endl;
		cout << "------Finished-------" << endl;

		std::string request_example2("POST scripts/test.sh HTTP/1.1\nHost: www.example.com\nContent-Type: application/x-www-form-urlencoded\nContent-Length: 27\n\nname=John+Doe&age=30&gender=M\n");
		CGI cgi2(request_example2);
		cout << "Expected output: \"Hello from test.sh!\"" << endl;
		cout << "Script output  : \"" << cgi2.response << "\"" << endl;
		cout << "---------------------" << endl;
		cout << "------Finished-------" << endl;
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return EXIT_SUCCESS;
} */