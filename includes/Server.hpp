#ifndef SERVER_HPP
#define SERVER_HPP

#include "User.hpp"
#include <map>
#include <vector>
#include <cstring>
#include <csignal>

using namespace std;

class User;

typedef struct
{
	string prefix;
	string command;
	string arguments;
} cmd;

enum log_level { INFO, WARN, ERROR };

class Server
{
	private:
		map<int,User> users;
		vector<pollfd> fds;
		const int port;
		const int max_clients;
		static volatile sig_atomic_t running;

		void handle_new_client();
		void handle_client_messages();
		void cleanup();
		void process_message(int clientFd, char *buffer);
		int create_socket();
		void execute_command(cmd cmd, User &user);
		void process_privmsg(cmd cmd, const User &user);
		string client_info(struct sockaddr_in &client_addr);
	public:
		Server(const int port);
		
		void main_loop();
		static void signal_handler(int signal);

		const User* getUser(int fd);
		const User* getUser(const string &nickname);

		void log(log_level level, const string &event, const string &details);

		void print_status();
};

#endif