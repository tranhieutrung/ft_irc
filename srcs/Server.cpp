#include <iostream>
#include <cstring>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <sstream>
#include <map>
#include "../includes/User.hpp"
#include "../includes/Server.hpp"

using namespace std;

void Server::execute_command(cmd cmd, User &user)
{
	cout << "Prefix: " << cmd.prefix << endl;
	cout << "Command: " << cmd.command << endl;
	cout << "Arguments: " << cmd.arguments << endl;

	if(cmd.command == "NICK")
		user.setNickname(cmd.arguments);
	if(cmd.command == "USER")
		user.setInfo(cmd.arguments);
	cout << "Print user: " << endl;
	cout << user << endl;
}

static cmd parse_line(string &message)
{
	cmd cmd;
	istringstream stream(message);
	if (message[0] == ':')
		stream >> cmd.prefix;
	stream >> cmd.command;
	getline(stream, cmd.arguments);
	return cmd;
}

void Server::handle_new_client()
{
	if (fds[0].revents & POLLIN)
	{
		int clientSocket = accept(fds[0].fd, nullptr, nullptr);
		if (clientSocket == -1)
			cerr << "Error accepting connection\n";
		else
		{
			cout << "New client connected!\n";
			pollfd new_pfd = {clientSocket, POLLIN, 0};
			fds.push_back(new_pfd);
			users[new_pfd.fd] = User(new_pfd.fd);
		}
	}
}

void Server::process_message(int clientFd, char *buffer)
{
	stringstream message;
	message << buffer;
	string line;
	while (getline(message, line))
		execute_command(parse_line(line), users[clientFd]);
}

void Server::handle_client_messages()
{
	for (size_t i = 1; i < fds.size(); i++)
	{
		if (fds[i].revents & POLLIN)
		{
			char buffer[1024] = {0};
			int bytesReceived = recv(fds[i].fd, buffer, sizeof(buffer), 0);

			if (bytesReceived > 0)
				process_message(fds[i].fd, buffer);
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

void Server::cleanup()
{
	for (pollfd pfd : fds)
	{
		close(pfd.fd);
	}
}

void Server::main_loop()
{
	while (cin)
	{
		if (poll(fds.data(), fds.size(), -1) < 0)
			throw runtime_error("Poll error");

		handle_new_client();
		handle_client_messages();
	}
	cleanup();
}

int Server::create_socket()
{
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
		throw runtime_error("Error creating socket");

	int opt = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
		throw runtime_error("Error binding socket");

	if (listen(serverSocket, max_clients) == -1)
		throw runtime_error("Error listening to socket");

	cout << "Server listening on port " << port << "...\n";
	return serverSocket;
}

Server::Server(const int port) : port(port), max_clients(10)
{
	fds.push_back({create_socket(), POLLIN, 0});
}