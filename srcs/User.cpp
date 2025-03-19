#include "User.hpp"
#include "Server.hpp"
#include <sstream>
#include <regex>

using namespace std;

User::User() :
	nickname("Unknown"),
	username(""),
	hostname(""),
	servername(""),
	realname(""),
	fd(-1),
	isOperator(false),
	isAuth(false) {}

User::User(int fd) :
	nickname("User" + to_string(fd -3)),
	username(""),
	hostname(""),
	servername(""),
	realname(""),
	fd(fd),
	isOperator(false),
	isAuth(false) {}

User::User(const User &other) :
	nickname(other.nickname),
	username(other.username),
	hostname(other.hostname),
	servername(other.servername),
	realname(other.realname),
	fd(other.fd),
	isOperator(other.isOperator),
	isAuth(other.isAuth) {}

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
	isAuth = other.isAuth;
	return *this;
}

int	User::setNickname(string &nickname)
{
	regex nick_regex(R"(^[A-Za-z\[\]\\`_^{}|][-A-Za-z0-9\[\]\\`_^{}|]{0,8}$)");
	if (regex_match(nickname, nick_regex) == false)
		return 1;
	this->nickname = nickname;
	return 0;
}

int User::setUsername(string &username)
{
	regex user_regex(R"(^[^\s@]{1,10}$)");
	if (regex_match(username, user_regex) == false)
		return 1;
	this->username = username;
	return 0;
}

int User::setHostname(string &hostname)
{
	regex host_regex(R"(^(?=.{1,255}$)([a-zA-Z0-9]([a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(\.[a-zA-Z0-9]{1,})*)$)");
	if (regex_match(hostname, host_regex) == false)
		return 1;
	this->hostname = hostname;
	return 0;
}

int User::setServername(string &servername)
{
	regex server_regex(R"(^(?=.{1,255}$)([a-zA-Z0-9]([a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(\.[a-zA-Z0-9]{1,})*)$)");
	if (regex_match(servername, server_regex) == false)
		return 1;
	this->servername = servername;
	return 0;
}

int User::setRealname(string &realname)
{
	regex real_regex(R"(^[\x20-\x7E]{1,50}$)");
	if (regex_match(realname, real_regex) == false)
		return 1;
	this->realname = realname;
	return 0;
}

int User::setInfo(string &args)
{
	string user, host, server, real;
	istringstream stream(args);
	stream >> user;
	stream >> host;
	stream >> server;
	getline(stream, real);
	if (setUsername(user) == 1)
		return 1;
	if (setHostname(host) == 1)
		return 1;
	if (setServername(server) == 1)
		return 1;
	return setRealname(real);
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

// void User::privmsg_channel(const Channel &channel, string &message)
// {
// 	for (User u : channel.users)
// 	{
// 		privmsg(u, message);
// 	}
// }

// void User::join(const string &channel)
// {
// 	string prefix = getFullIdentifier();
// 	(void) channel;
// 	// server is going to send everyone in this channel "<prefix> JOIN #<channel>"
// }

void	User::setAuth(bool status) {
	this->isAuth = status;
}

bool User::getAuth() {
	return (this->isAuth);
}
