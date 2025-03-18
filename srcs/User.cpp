#include "../includes/User.hpp"
#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"
#include "../includes/ErrorCodes.hpp"
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
	nickname("User" + to_string(fd)),
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
		return ERR_ERRONEUSNICKNAME;
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
	if (setUsername(user) == ERR_ERRONEUSNICKNAME)
		return ERR_ERRONEUSNICKNAME;
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

int User::privmsg(const User &recipient, string &message)
{
	if (message.empty())
		return ERR_NOTEXTTOSEND;
	string prefix = getFullIdentifier();
	stringstream stream;
	stream << prefix << " PRIVMSG " << recipient.getNickname() << " " << message << "\r\n";
	const char *buffer = stream.str().c_str();
	send(recipient.fd, buffer, sizeof(buffer), 0);
	return 0;
}

int User::privmsg(const Channel &channel, string &message)
{
	for (const auto &pair : channel.getUserList())
	{
		int ret = privmsg(pair.second, message);
		if (ret != 0)
			return ret;
	}
	return 0;
}

// void User::join(const string &channel)
// {
// 	string prefix = getFullIdentifier();
// 	(void) channel;
// 	// server is going to send everyone in this channel "<prefix> JOIN #<channel>"
// }

void	User::setAuth(bool status) {
	this->isAuth = status;
}
