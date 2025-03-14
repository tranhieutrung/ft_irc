#include "../includes/Server.hpp"
#include <iostream>

using namespace std;

int main()
{
	int serverSocket = server_socket();
	if (serverSocket == -1)
	{
		cerr << "Couldn't create socket\n";
		return (1);
	}

	main_loop(serverSocket);
	return 0;
}