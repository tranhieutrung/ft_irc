#include "../includes/Server.hpp"
#include <iostream>

using namespace std;

void validate_args(int ac, char **av) {

	if (ac != 3) {
		cerr << "Error: invalid arguments!" << endl;
		cerr << "Usage: ./ircserv <port> <password>" << endl;
		exit (EXIT_FAILURE);
	}
	int port = atoi(av[1]);
	if ((port < 6660 || port > 6669) && port != 6697) {
		cerr << "Error: invalid port!" << endl;
		cerr << "Usage: valid port range: 6660-6669 or 6697" << endl;
		exit (EXIT_FAILURE);
	}
}

int main(int ac, char **av) {
	validate_args(ac, av);
	Server server = Server(av[1], av[2]);
	server.main_loop();
	return 0;
}
