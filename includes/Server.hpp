#ifndef SERVER_HPP
#define SERVER_HPP

#include "User.hpp"
#include <map>
#include <vector>
#include <cstring>
#include <csignal>
#include <memory>  // unique_ptr
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
		// const int port; trung
		// const int max_clients; trung
		static volatile sig_atomic_t running;
		
		const string	_name = "IRCS";
		const int		_port;
		const string 	_password;
		const int		_maxClients = 1024;
		// int				_serverSocket;

		void handleNewClient();
		void handleClientMessages(int index);
		void cleanup();
		void process_message(int clientFd, string buffer);
		// int create_socket();
		int createSocket();
		void execute_command(cmd cmd, User &user);
		void process_privmsg(cmd cmd, const User &user);
		string client_info(struct sockaddr_in &client_addr);

		string	_processPASS(cmd cmd, User &user);
		string	_processNICK(cmd cmd, User &user);
		string	_processUSER(cmd cmd, User &user);
		string	_processOPER(cmd cmd, User &user);
		string	_processMODE(cmd cmd, User &user);
		string	_processPRIVMSG(cmd cmd, User &user);
		string	_processJOIN(cmd cmd, User &user);
		string	_processTOPIC(cmd cmd, User &user);
		string	_processKICK(cmd cmd, User &user);
		string	_processQUIT(cmd cmd, User &user);

	public:
		// Server(const int port); Trung
		Server(std::string port, std::string password);
		~Server();

		void 			start();
		static void 	signal_handler(int signal);

		const User*		getUser(int fd);
		const User*		getUser(const string &nickname);

		void			log(log_level level, const string &event, const string &details);

		void			print_status();
};

#endif