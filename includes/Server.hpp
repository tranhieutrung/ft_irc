#ifndef SERVER_HPP
#define SERVER_HPP

#include "../includes/IO.hpp"
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
#include "User.hpp"
#include <csignal>
#include "Channel.hpp"
#include "ErrorCodes.hpp"
#include "ReplyCodes.hpp"

using namespace std;

class User;

enum log_level { INFO, WARN, ERROR };

class Server
{
	private:
		map<int, User> users;
		map<string, Channel> channels;
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

		// helper functions:
		bool	_nickIsUsed(string nick);
		bool	_userIsUsed(string username);
		// Commands
		int	PASS(cmd cmd, User &user);
		int	NICK(cmd cmd, User &user);
		int	USER(cmd cmd, User &user);
		int	JOIN(cmd cmd, User &user);
		int	PING(cmd cmd, User &user);
		int	PONG(cmd cmd, User &user);
		int	OPER(cmd cmd, User &user);
		int	PRIVMSG(cmd cmd, User &user);
		int	QUIT(cmd cmd, User &user);
		int	PART(cmd cmd, User &user);

		//channel commands
		int	KICK(cmd cmd, User &user);
		int	INVITE(cmd cmd, User &user);
		int	TOPIC(cmd cmd, User &user);
		int	MODE(cmd cmd, User &user);

		string	commandResponses(int code, cmd cmd, User &user);
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

		std::map<string, Channel>::iterator findChannel (string channel);
};

#endif
