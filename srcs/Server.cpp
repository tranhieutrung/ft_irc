#include <iostream>
#include <cstring>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <sstream>
#include "../includes/User.hpp"
#include "../includes/Server.hpp"

using namespace std;

#define PORT 8090
#define MAX_CLIENTS 10

vector<User> users;

void do_command(string &prefix, string &command, string &arguments, pollfd &pfd)
{
	(void) pfd;
	cout << "do_command" << endl;
	cout << "Prefix: " << prefix << endl;
	cout << "Command: " << command << endl;
	cout << "Arguments: " << arguments << endl;
}

string handle_line(string &message, pollfd &pfd)
{
	size_t nlpos = message.find("\n");

	string prefix, command, arguments;
	istringstream stream(message);
	if (message[0] == ':')
		stream >> prefix;
	stream >> command;
	getline(stream, arguments);

	cout << "Message: " << message << endl;
	do_command(prefix, command, arguments, pfd);

	if (nlpos == string::npos)
		return "";
	return message.substr(nlpos + 1, message.size() - nlpos - 1);
}

void main_loop(int serverSocket)
{
	vector<pollfd> fds;
	fds.push_back({serverSocket, POLLIN, 0});

	while (true)
	{
		int activity = poll(fds.data(), fds.size(), -1);
		if (activity < 0)
		{
			cerr << "Poll error\n";
			break;
		}

		if (fds[0].revents & POLLIN)
		{
			int clientSocket = accept(serverSocket, nullptr, nullptr);
			if (clientSocket == -1)
				cerr << "Error accepting connection\n";
			else
			{
				cout << "New client connected!\n";
				fds.push_back({clientSocket, POLLIN, 0});
			}
		}

		for (size_t i = 1; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				char buffer[1024] = {0};
				int bytesReceived = recv(fds[i].fd, buffer, sizeof(buffer), 0);

				if (bytesReceived > 0)
				{
					string message = string(buffer);
					cout << "------------" << endl;
					while (!message.empty())
						message = handle_line(message, fds[i]); // read one line and return the rest of the message
				}
				else
				{
					cout << "Client disconnected\n";
					close(fds[i].fd);
					fds.erase(fds.begin() + i);
					i--;
				}
			}
		}
	}

	close(serverSocket);
}

int server_socket()
{
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
	{
		cerr << "Error creating socket\n";
		return -1;
	}

	int opt = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
	{
		cerr << "Error binding socket\n";
		return -1;
	}

	if (listen(serverSocket, MAX_CLIENTS) == -1)
	{
		cerr << "Error listening on socket\n";
		return -1;
	}

	cout << "Server listening on port " << PORT << "...\n";
	return serverSocket;
}