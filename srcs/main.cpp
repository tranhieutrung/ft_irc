#include "../includes/Server.hpp"
#include <iostream>

using namespace std;
#define PORT 8080

int main()
{
	Server server = Server(PORT);
	server.main_loop();
	return 0;
}