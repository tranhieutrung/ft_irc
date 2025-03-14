#ifndef SERVER_HPP
#define SERVER_HPP

#include "User.hpp"
#include <map>
#include <vector>
#include <cstring>

using namespace std;

class User;

typedef struct
{
	string prefix;
	string command;
	string arguments;
} cmd;

class Server
{
	private:
		map<int,User> users;
		vector<pollfd> fds;
		const int port;
		const int max_clients;

		void handle_new_client();
		void handle_client_messages();
		void cleanup();
		void process_message(int clientFd, char *buffer);
		int create_socket();
		void execute_command(cmd cmd, User &user);
	public:
		Server(const int port);

		void main_loop();

		const User* getUser(int fd);
		const User* getUser(const string &nickname);

		void print_status();
};

#endif