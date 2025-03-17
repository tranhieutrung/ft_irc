#ifndef SERVER_HPP
#define SERVER_HPP

#include "User.hpp"
#include <map>
#include <vector>
#include <cstring>
#include <csignal>
#include <memory>  // unique_ptr

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
		// const int port; trung
		// const int max_clients; trung
		static volatile sig_atomic_t running;
		
		string			_name;
		const int		_port;
		const string 	_password;
		const int		_maxClients = 1024;

		void handle_new_client();
		void handle_client_messages();
		void cleanup();
		void process_message(int clientFd, char *buffer);
		// int create_socket();
		int createSocket();
		void execute_command(cmd cmd, User &user);
		void process_privmsg(cmd cmd, const User &user);
		string client_info(struct sockaddr_in &client_addr);
	public:
		// Server(const int port); Trung
		Server(std::string port, std::string password);
		
		void main_loop();
		static void signal_handler(int signal);

		const User* getUser(int fd);
		const User* getUser(const string &nickname);

		void log(log_level level, const string &event, const string &details);

		void print_status();
};

#endif