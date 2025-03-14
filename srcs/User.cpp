#include "User.hpp"
#include "Server.hpp"
#include <sstream>

using namespace std;

User::User() : nickname(""), username(""), hostname(""), servername(""), realname(""), fd(-1), isOperator(false) {}

User::User(int fd) : nickname(""), username(""), hostname(""), servername(""), realname(""), fd(fd), isOperator(false) {}

User::User(const User &other) :
						nickname(other.nickname),
						username(other.username),
						hostname(other.hostname),
						servername(other.servername),
						realname(other.realname),
						fd(other.fd),
						isOperator(other.isOperator) {}

User& User::operator=(const User &other)
{
	if (this == &other)
		return *this;
	nickname = other.nickname;
	username = other.username;
	hostname = other.hostname;
	servername = other.servername;
	realname = other.realname;
	fd = other.fd;
	isOperator = other.isOperator;
	return *this;
}

void User::setInfo(string &args)
{
	istringstream stream(args);
	stream >> username;
	stream >> hostname;
	stream >> servername;
	getline(stream, realname);
}

ostream& operator<<(ostream &os, const User &user)
{
	os << "Nickname: " << user.getNickname() << endl;
	os << "Username: " << user.getUsername() << endl;
	os << "Hostname: " << user.getHostname() << endl;
	os << "Servername: " << user.getServername() << endl;
	os << "Realname: " << user.getRealname() << endl;
	os << "fd: " << user.getFd() << endl;
	os << "isOperator: " << user.getIsOperator() << endl;
	return os;
}

string User::getFullIdentifier() const
{
	return ":" + nickname + "!" + username + "@" + hostname;
}

void User::privmsg(const User &recipient, string &message)
{
	string prefix = getFullIdentifier();
	(void) recipient;
	(void) message;
	// server is gonna send the recipient client "<prefix> PRIVMSG <recipient nick> :<message>"
}