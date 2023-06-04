#include "libwebserv.hpp"

using std::cerr;
using std::cout;
using std::endl;

void printStartUpMessage(void) {
    system("clear");
    cout << "42_Webserv version: 42_Webserv/1.0\n"
         << "Compliant with the c++98 standard\n"
         << "Configure arguments: \"-Wall -Wextra -Werror -std=c++98\"\n\n"
         << getTimeStamp() << endl;
}

int main(int argc, char** argv) {
    if (argc > 3)
        return panic(ARGC_ERR);

    std::string configFile;
    if (argc == 1)
        configFile = "config/default.conf";
    else {
        if (!argv[1][0])
            return panic(NO_CONFIG_FILE_ERR);
        configFile = argv[1];
    }

    try {
        printStartUpMessage();
        WebServ webserv(configFile);
        webserv.bootServers();
        webserv.runServers();
        return EXIT_SUCCESS;
    } catch (std::exception& e) {
        cerr << ERROR_MSG_PREFFIX << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
