#include "../includes/Server.hpp"
#include <iostream>

using namespace std;

bool isValidPassword(const string& s) {
	if (s.empty())
		return false;
	// Regex: only a-z, A-Z, 0-9, and !@#$%^&*_.-~
	std::regex pattern("^[a-zA-Z0-9!@#\\$%\\^&\\*_.\\-~]+$");
	return std::regex_match(s, pattern);
}

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
	if (!isValidPassword(av[2])) {
		cerr << "Error: invalid password!" << endl;
		exit (EXIT_FAILURE);
	}
}

int main(int ac, char **av) {
	validate_args(ac, av);
	try {
		Server server(av[1], av[2]);
		server.start();
	} catch (const exception &e) {
		cerr << "Error: " << e.what() << endl;
		return (EXIT_FAILURE);
	}

	return 0;
}
