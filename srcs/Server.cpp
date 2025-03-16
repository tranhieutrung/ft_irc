#include <iostream>
#include <cstring>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <sstream>
#include <map>
#include <iomanip>
#include <ctime>
#include "../includes/User.hpp"
#include "../includes/Server.hpp"
#include <csignal>

using namespace std;

volatile sig_atomic_t Server::running = 1;

void Server::process_privmsg(cmd cmd, const User &user)
{
	string target;
	string message;
	istringstream stream(cmd.arguments);
	stream >> target;
	getline(stream, message);

	(void) user;

	// if (target[0] == '#')
	// {
	// 	const Channel *recipient = getChannel(target);
	// 	if (recipient != nullptr)
	// 		user.privmsg(*recipient, message);
	// 	else
	// 		cout << target << " not found" << endl;
	// }
	// else
	// {
	// 	const User *recipient = getUser(target);
	// 	if (recipient != nullptr)
	// 		user.privmsg(*recipient, message);
	// 	else
	// 		cout << target << " not found" << endl;
	// }
}

int Server::check_password(const string &args)
{
	if (args == password)
		return 0;
	return 1;
}

void Server::execute_command(cmd cmd, User &user)
{
	int code = 2; // 2 = unknown cmd, 1 = error, 0 = success
	if(cmd.command == "NICK")
	{
		if (getUser(cmd.arguments) == nullptr)
			code = user.setNickname(cmd.arguments);
		else
			code = 1;
	}
	if(cmd.command == "USER")
		code = user.setInfo(cmd.arguments);
	if (cmd.command == "PASS")
		code = check_password(cmd.arguments);
	// if(cmd.command == "CAP" && cmd.arguments == "LS")
	// 	send_cap_ls();
	// if(cmd.command == "PRIVMSG")
	// 	process_privmsg(cmd, user);
	// if(cmd.command == "JOIN")
	// 	user.join(cmd.arguments);
	switch (code)
	{
		case 2:
			log(WARN, "Command", "Unknown command received: " + cmd.command);
			break;
		case 1:
			log(ERROR, "Command", "Could not execute command: " + cmd.command);
			break;
		case 0:
			log(INFO, "Command", "User \"" + user.getNickname() + "\" executed command " + cmd.command);
			break;
	}
}

static cmd parse_line(string &message)
{
	cmd cmd;
	istringstream stream(message);
	if (message[0] == ':')
		stream >> cmd.prefix;
	stream >> cmd.command;
	stream.ignore(1); // skip space
	getline(stream, cmd.arguments, '\r');
	return cmd;
}

string Server::client_info(struct sockaddr_in &client_addr)
{
	return "IP: " + string(inet_ntoa(client_addr.sin_addr)) 
	+ " Port: " + to_string(ntohs(client_addr.sin_port));
}

void Server::handle_new_client()
{
	if (fds[0].revents & POLLIN)
	{
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		int clientSocket = accept(fds[0].fd, (struct sockaddr *)&client_addr, &client_len);
		if (clientSocket == -1)
		{
			log(ERROR, "Connection", "Error accepting connection " + client_info(client_addr));
			cerr << "Error accepting connection" << endl;
			return;
		}
		pollfd new_pfd = {clientSocket, POLLIN, 0};
		fds.push_back(new_pfd);
		users[new_pfd.fd] = User(new_pfd.fd);
		log(INFO, "Connection", "New client connected: " + client_info(client_addr));
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
				log(INFO, "Connection", "Client disconnected: " + users[fds[i].fd].getNickname());
				close(fds[i].fd);
				users.erase(fds[i].fd);
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

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	while (running)
	{
		if (poll(fds.data(), fds.size(), -1) < 0) 
		{
			if (errno != EINTR) {
				log(ERROR, "Server", "Poll error: " + std::string(strerror(errno)));
				throw runtime_error("Poll error");
			}
		}
		handle_new_client();
		handle_client_messages();
	}
	cleanup();
	log(INFO, "Server", "Shutting down server");
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

	log(INFO, "Server", "Server started on port " + to_string(port));
	return serverSocket;
}

Server::Server(const int port, const string &password) : port(port), max_clients(10), password(password)
{
	fds.push_back({create_socket(), POLLIN, 0});
}

const User* Server::getUser(const string &nickname)
{
	for (const auto &pair : users)
	{
		if (pair.second.getNickname() == nickname)
			return &pair.second;
	}
	return nullptr;
}

const User* Server::getUser(int fd)
{
	if (users.find(fd) != users.end())
		return &users[fd];
	return nullptr;
}

void Server::log(log_level level, const string &event, const string &details)
{
	time_t now = time(nullptr);
	tm *ltm = localtime(&now);

	const string RESET = "\033[0m";
	const string RED = "\033[31m";
	const string ORANGE = "\033[38;5;214m";
	const string GREEN = "\033[32m";

	cout << "[" << put_time(ltm, "%d.%m.%Y %H:%M:%S") << "] ";
	switch (level)
	{
		case INFO:
			cout << GREEN;
			cout << "[INFO] ";
			break;
		case WARN:
			cout << ORANGE;
			cout << "[WARN] ";
			break;
		case ERROR:
			cout << RED;
			cout << "[ERROR] ";
			break;
	}
	cout << RESET;
	cout << "[" << event << "] " << details << endl;
}

void Server::signal_handler(int signal)
{
	if (signal == SIGINT || signal == SIGTERM)
		running = 0;
}