#ifndef SERVER_HPP
#define SERVER_HPP

#include "User.hpp"
#include <map>
#include <vector>
#include <cstring>

using namespace std;

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
	
	public:
		Server(const int port);

		void main_loop();
		int create_socket();
		void do_command(cmd cmd, User &user);
		string handle_line(string &message, User &user);

		User &getUser(int fd) { return users[fd]; }
};

#endif