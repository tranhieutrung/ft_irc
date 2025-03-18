#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <poll.h>
#include <iostream>
#include "Server.hpp"

class Channel;

using namespace std;

class User
{
	private:
		string nickname, username, hostname, servername, realname;
		int fd;
		bool isOperator;
		bool isAuth;

	public:
		// constructors
		User();
		User(int fd);
		User(const User &other);
		User &operator=(const User &other);

		int privmsg(const User &recipient, string &args);
		int privmsg(const Channel &reci_chan, string &message);

		// getters
		string getNickname() const { return nickname; }
		string getUsername() const { return username; }
		string getHostname() const { return hostname; }
		string getServername() const { return servername; }
		string getRealname() const { return realname; }
		int getFd() const { return fd; }
		bool getIsOperator() const { return isOperator; }
		string getFullIdentifier() const;

		// setters
		int setInfo(string &args);
		int setNickname(string &nickname);
		int setUsername(string &username);
		int setHostname(string &hostname);
		int setServername(string &servername);
		int setRealname(string &realname);
		void setIsOperator(bool isOperator) { this->isOperator = isOperator; }

		void	setAuth(bool status);
		
		friend ostream &operator<<(ostream &os, const User &user);
};


#endif