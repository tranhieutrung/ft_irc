#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <poll.h>
#include <iostream>
#include "Server.hpp"

class Channel;

class User
{
	private:
		std::string nickname, username, hostname, servername, realname;
		const int fd;
		bool isOperator;
		bool isAuth;
		std::vector<Channel> channels;
	public:
		// constructors
		User();
		User(const int fd);
		User(const User &other);
		User &operator=(const User &other);

		int privmsg(const User &recipient, const std::string &message) const;
		int privmsg(const Channel &reci_chan, const std::string &message) const;
		int join(Channel &channel);
		int join(Channel &channel, const string &password);

		// getters
		std::string getNickname() const { return nickname; }
		std::string getUsername() const { return username; }
		std::string getHostname() const { return hostname; }
		std::string getServername() const { return servername; }
		std::string getRealname() const { return realname; }
		int getFd() const { return fd; }
		bool getIsOperator() const { return isOperator; }
		std::string getFullIdentifier() const;
		bool getAuth() const;

		// setters
		int setInfo(const std::string &args);
		int setNickname(const std::string &nickname);
		int setUsername(const std::string &username);
		int setHostname(const std::string &hostname);
		int setServername(const std::string &servername);
		int setRealname(const std::string &realname);
		void setIsOperator(const bool isOperator) { this->isOperator = isOperator; }
		void setIsAuth(const bool status);
		
		friend ostream &operator<<(ostream &os, const User &user);
};


#endif