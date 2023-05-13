#include "libwebserv.hpp"

using std::cerr;
using std::cout;
using std::endl;

void printStartUpMessage(void) {
    system("clear");
    cout << "42_Webserv version: 42_Webserv/1.0\n"
         << "Compliant with the c++98 standard\n"
         << "Configure arguments: \"-Wall -Wextra -Werror -std=c++98 -g -fsanitize=address\"\n\n"
         << getTimeStamp() << endl;
}

int main(int argc, char** argv) {
    if (argc != 2 || !argv[1][0])
        return panic(ARGS_ERR);

    try {
        printStartUpMessage();
        WebServ webserv(argv[1]);
        webserv.bootServers();
        webserv.runServers();
        return EXIT_SUCCESS;
    } catch (std::exception& e) {
        cerr << ERROR_MSG_PREFFIX << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
