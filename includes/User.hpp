#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <poll.h>
#include <iostream>

using namespace std;

class User
{
	private:
		string nickname, username, hostname, servername, realname;
		int fd;
		bool isOperator;
	public:
		// constructors
		User();
		User(int fd);
		User(const User &other);
		User &operator=(const User &other);

		// getters
		string getNickname() const { return nickname; }
		string getUsername() const { return username; }
		string getHostname() const { return hostname; }
		string getServername() const { return servername; }
		string getRealname() const { return realname; }
		int getFd() const { return fd; }
		bool getIsOperator() const { return isOperator; }

		// setters
		void setInfo(string &args);
		void setNickname(string nickname) { this->nickname = nickname; }
		void setUsername(string username) { this->username = username; }
		void setHostname(string hostname) { this->hostname = hostname; }
		void setServername(string servername) { this->servername = servername; }
		void setRealname(string realname) { this->realname = realname; }
		void setIsOperator(bool isOperator) { this->isOperator = isOperator; }
};

ostream &operator<<(ostream &os, User &user);

#endif