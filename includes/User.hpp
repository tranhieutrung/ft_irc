#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <poll.h>
#include <iostream>

using namespace std;

class User
{
	private:
		string nickname, username, hostname, domain, realname;
		pollfd pfd;
		bool isOperator;
	public:
		User(pollfd pfd) : nickname(""), username(""), hostname(""), domain(""), realname(""), isOperator(false) { cout << "User created with pfd: " << pfd.fd << endl; }
		// getters
		string getNickname() const { return nickname; }
		string getUsername() const { return username; }
		string getHostname() const { return hostname; }
		string getDomain() const { return domain; }
		string getRealname() const { return realname; }
		pollfd getPfd() const { return pfd; }
		bool getIsOperator() const { return isOperator; }

		// setters
		void setNickname(string nickname) { this->nickname = nickname; }
		void setUsername(string username) { this->username = username; }
		void setHostname(string hostname) { this->hostname = hostname; }
		void setDomain(string domain) { this->domain = domain; }
		void setRealname(string realname) { this->realname = realname; }
		void setIsOperator(bool isOperator) { this->isOperator = isOperator; }
};

#endif