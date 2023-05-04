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
		/*HTTP Header reference
			POST scripts/test.py HTTP/1.1
			Host: www.example.com
			Content-Type: application/x-www-form-urlencoded
			Content-Length: 27

			name=John+Doe&age=30&gender=M
		*/

	try
	{
		cout << "--------Debug--------" << endl;
		cout << "---------------------" << endl;
		setenv("REQUEST_METHOD", "POST", 1);
		setenv("SCRIPT_NAME", "scripts/test.sh", 1);
		// setenv("PATH_INFO", "/nfs/homes/jarsenio/Desktop/projects/webserver/lvl_5_webserv", 1);
		setenv("PATH_INFO", "/home/Flirt/Desktop/Projects_42/Webserver/lvl_5_webserv", 1);
		setenv("QUERY_STRING", "name=John+Doe+Wallfrost+dos+Santos&age=31&gender=M", 1);
		setenv("CONTENT_LENGTH", "7", 1);
		setenv("CONTENT_TYPE", "random", 1);

		CGI cgi2;
		std::ifstream output(".output");
		std::string line;
		cout << "Expected output: \"Hello from test.sh!\"" << endl;
		cout << "Script output  : \"";
		while (getline(output, line))
			std::cout << line;
		cout << "\"" << endl;
		cout << "---------------------" << endl;
		cout << "------Finished-------" << endl;
		output.close();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	try
	{
		cout << "--------Debug--------" << endl;
		cout << "---------------------" << endl;
		setenv("REQUEST_METHOD", "POST", 1);
		setenv("SCRIPT_NAME", "scripts/story_params.py", 1);
		// setenv("PATH_INFO", "/nfs/homes/jarsenio/Desktop/projects/webserver/lvl_5_webserv", 1);
		setenv("PATH_INFO", "/home/Flirt/Desktop/Projects_42/Webserver/lvl_5_webserv", 1);
		setenv("QUERY_STRING", "name=John+Doe+Santos&age=21&gender=M", 1);
		setenv("CONTENT_LENGTH", "7", 1);
		setenv("CONTENT_TYPE", "random", 1);

		CGI cgi;
		// cout << "Expected output: \"Hello from test.py!\"" << endl;
		std::ifstream output(".output");
		std::string line;
		cout << "Script output  : \"";
		while (getline(output, line))
			std::cout << line;
		cout << "\"" << endl;
		cout << "---------------------" << endl;
		cout << "------Finished-------" << endl;
		output.close();
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}

	return EXIT_SUCCESS;
}