#ifndef USER_HPP
#define USER_HPP

#include "Server.hpp"
#include <string>
#include <poll.h>
#include <iostream>
#include <map>

class Channel;

class User
{
	private:
		std::string nickname, username, hostname, servername, realname;
		int fd;
		bool isOperator;
		bool isAuth;
		bool nickIsSet;
		bool userIsSet;
		bool isRegistered;

		// std::map<std::string, Channel> joinedChannels;
	public:
		// constructors
		User();
		User(const int fd);
		User(const User &other);
		User &operator=(const User &other);

		bool isInChannel(const std::string &channelName) const;
		int privmsg(const User &recipient, const std::string &message) const;
		int privmsg(const Channel &reci_chan, const std::string &message) const;
		int join(Channel &channel);
		int join(Channel &channel, const std::string &password);
		int part(Channel &channel, const std::string &message);
		int quit(const std::string &message);

		// getters
		std::string getNickname() const { return nickname; }
		std::string getUsername() const { return username; }
		std::string getHostname() const { return hostname; }
		std::string getServername() const { return servername; }
		std::string getRealname() const { return realname; }
		int getFd() const { return fd; }
		bool getIsOperator() const { return isOperator; }
		std::string getFullIdentifier() const;
		bool getAuth() const { return isAuth; }
		bool getNickIsSet() const { return nickIsSet; }
		bool getUserIsSet() const { return userIsSet; }
		bool getIsRegistered() const { return isRegistered; }

		// setters
		int setNickname(const std::string &nickname);
		int setUsername(const std::string &username);
		int setHostname(const std::string &hostname);
		int setServername(const std::string &servername);
		int setRealname(const std::string &realname);
		void setIsOperator(const bool isOperator) { this->isOperator = isOperator; }
		void setAuth(const bool status) { isAuth = status; }
		void setNickIsSet(const bool status) { nickIsSet = status; }
		void setUserIsSet(const bool status) { userIsSet = status; }
		void setIsRegistered(const bool status) { isRegistered = status; }

		friend bool operator==(const User &lhs, const User &rhs);
		friend bool operator!=(const User &lhs, const User &rhs);
};


#endif