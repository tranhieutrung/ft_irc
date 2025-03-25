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
		int fd;
		bool isOperator;
		bool isAuth;
		bool nickIsSet;
		bool userIsSet;
		bool isRegistered;

		std::map<std::string, Channel> joinedChannels;
	public:
		// constructors
		User();
		User(const int fd);
		User(const User &other);
		User &operator=(const User &other);

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
		bool getAuth() const;
		bool getNickIsSet() const;
		bool getUserIsSet() const;
		bool getIsRegistered() const;

		// setters
		int setInfo(const std::string &args);
		int setNickname(const std::string &nickname);
		int setUsername(const std::string &username);
		int setHostname(const std::string &hostname);
		int setServername(const std::string &servername);
		int setRealname(const std::string &realname);
		void setIsOperator(const bool isOperator) { this->isOperator = isOperator; }
		void setAuth(const bool status);
		void setNickIsSet(const bool status);
		void setUserIsSet(const bool status);
		void setIsRegistered(const bool status);

		friend ostream &operator<<(ostream &os, const User &user);
		friend bool operator==(const User &lhs, const User &rhs);
};


#endif
