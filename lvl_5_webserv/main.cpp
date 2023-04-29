#include "libwebserv.hpp"

using std::cout;
using std::cerr;
using std::endl;

// int main(int argc, char **argv)
// {
// 	system("clear");
// 	if (argc != 2 || !argv[1][0])
// 		return panic(ARGS_ERR);

// 	try {
// 		cout << getTimeStamp() << endl;
// 		WebServ webserv(argv[1]);
// 		webserv.bootServers();
// 		webserv.runServers();
// 		return EXIT_SUCCESS;
// 	}
// 	catch (std::exception& e) {
// 		cerr << ERROR_MSG_PREFFIX << e.what() << endl;
// 		return EXIT_FAILURE;
// 	}
// }

int main(void)
{
	//TODO: check if all exceptions are working
	//TODO: check if script receives arguments
	//TODO: Python script not running but bash is
	//TODO: Understand GET POST DELETE

	try
	{
		/*HTTP Header reference
			POST scripts/test.py HTTP/1.1
			Host: www.example.com
			Content-Type: application/x-www-form-urlencoded
			Content-Length: 27

			name=John+Doe&age=30&gender=M
		*/

		cout << "--------Debug--------" << endl;
		cout << "---------------------" << endl;
		setenv("REQUEST_METHOD", "GET", 0);
		setenv("SCRIPT_NAME", "scripts/story_params.py", 0);
		// setenv("PATH_INFO", "/nfs/homes/roramos/Documents/42_webserv/lvl_5_webserv", 0);
		setenv("PATH_INFO", "/home/Flirt/Desktop/Projects_42/Webserver/lvl_5_webserv", 0);
		setenv("QUERY_STRING", "name=John+Doe&age=30&gender=M", 0);

		CGI cgi;
		// cout << "Expected output: \"Hello from test.py!\"" << endl;
		cout << "Script output  : \"" << cgi.response << "\"" << endl;
		cout << "---------------------" << endl;
		cout << "------Finished-------" << endl;

		setenv("SCRIPT_NAME", "scripts/test.sh", 1);
		CGI cgi2;
		cout << "Expected output: \"Hello from test.sh!\"" << endl;
		cout << "Script output  : \"" << cgi2.response << "\"" << endl;
		cout << "---------------------" << endl;
		cout << "------Finished-------" << endl;
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return EXIT_SUCCESS;
}