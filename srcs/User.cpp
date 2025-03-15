#include "User.hpp"
#include "Server.hpp"
#include <sstream>
#include <regex>

using namespace std;

User::User() : nickname("Unknown"), username(""), hostname(""), servername(""), realname(""), fd(-1), isOperator(false) {}

User::User(int fd) : nickname("User" + to_string(fd)), username(""), hostname(""), servername(""), realname(""), fd(fd), isOperator(false) {}

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

int	User::setNickname(string &nickname)
{
	regex nick_regex("^[A-Za-z\\[\\]\\\\`_^{}|][A-Za-z0-9\\[\\]\\\\`_^{}|-]{0,8}$");
	if (regex_match(nickname, nick_regex) == false)
		return 1;
	this->nickname = nickname;
	return 0;
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