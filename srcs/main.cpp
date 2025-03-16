#include "../includes/Server.hpp"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		cerr << "Usage: " << argv[0] << " <port> <password>" << endl;
		return 1;
	}
	int port = atoi(argv[1]);
	string password = string(argv[2]);
	Server server = Server(port, password);
	server.main_loop();
	return 0;
}