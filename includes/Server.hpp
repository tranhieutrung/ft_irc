#ifndef SERVER_HPP
#define SERVER_HPP

#include "../includes/IO.hpp"
#include <map>
#include <vector>
#include <cstring>
#include <csignal>
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
#include <regex>
#include "Utils.hpp"

using namespace std;

class User;

class Server
{
	private:
		map<int, User>					users;
		map<string, Channel>			channels;
		vector<pollfd>					fds;
		static volatile sig_atomic_t	running;
		const string					_name = "IRCS";
		const int						_port;
		const string					_password;
		const int						_maxClients = 1024;

		void 	handleNewClient();
		void 	handleClientMessages(size_t *index);
		void 	cleanup();
		void 	process_message(int clientFd, string buffer);
		int		createSocket();
		void 	execute_command(cmd cmd, User &user);
		void 	process_privmsg(cmd cmd, const User &user);
		string 	client_info(struct sockaddr_in &client_addr);

		// helper functions:
		bool	_nickIsUsed(string nick);
		bool	_userIsUsed(string username);

		// Commands
		int		PASS(cmd cmd, User &user);
		int		NICK(cmd cmd, User &user);
		int		USER(cmd cmd, User &user);
		int		JOIN(cmd cmd, User &user);
		int		PING(cmd cmd, User &user);
		int		PONG(cmd cmd, User &user);
		// int		OPER(cmd cmd, User &user);
		int		PRIVMSG(cmd cmd, User &user);
		int		QUIT(cmd cmd, User &user);
		int		PART(cmd cmd, User &user);
		int		WHOIS(cmd cmd, User &user);

		//channel commands
		int		KICK(cmd cmd, User &user);
		int		INVITE(cmd cmd, User &user);
		int		TOPIC(cmd cmd, User &user);
		int		MODE(cmd cmd, User &user);

		string	createMessage(int code, cmd cmd, User &user);
		string	createMessage(int code, cmd cmd, User &user, Channel &channel);
		int 	createChannel(Channel*& channel, User &user, const std::string &channelName, const std::string &key);
		Channel*	findChannelByName(const std::string& channelName);
		User* 	findUserByNickName(const string& nickName);
		void 	sendMessage(int code, cmd cmd, User &user);
		void 	sendMessage(int code, cmd cmd, User &user, Channel &channel);
		void 	removeUser(int UserFd);
		void	partAll(const User &user);

	public:
		Server(std::string port, std::string password);
		~Server();

		void 			start();
		static void 	signal_handler(int signal);

		const User*		getUser(int fd);
		const User*		getUser(const string &nickname);
		
		std::optional<std::map<string, Channel>::iterator> findChannel(string channel);
};
	
#endif