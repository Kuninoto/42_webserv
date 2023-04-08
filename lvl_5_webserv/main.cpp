#include "libwebserv.hpp"

int main(int argc, char **argv)
{
	if (argc != 2 || !argv[1][0])
		return panic(ARGS_ERR);

	return EXIT_SUCCESS;
}
