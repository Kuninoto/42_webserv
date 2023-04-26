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
		webserv.bootServers();
		webserv.runServers();
		return EXIT_SUCCESS;
	}
	catch (std::exception& e) {
		cerr << ERROR_MSG_PREFFIX << e.what() << endl;
		return EXIT_FAILURE;
	}
}
