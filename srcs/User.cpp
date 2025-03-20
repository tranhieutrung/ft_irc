#include "../includes/User.hpp"
#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"
#include "../includes/ErrorCodes.hpp"
#include "../includes/IO.hpp"
#include <sstream>
#include <regex>
#include <vector>

User::User() :
	nickname("Unknown"),
	username(""),
	hostname(""),
	servername(""),
	realname(""),
	fd(-1),
	isOperator(false),
	isAuth(false) {}

User::User(const int fd) :
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

int	User::setNickname(const std::string &nickname)
{
	regex nick_regex(R"(^[A-Za-z\[\]\\`_^{}|][-A-Za-z0-9\[\]\\`_^{}|]{0,8}$)");
	if (regex_match(nickname, nick_regex) == false)
		return ERR_ERRONEUSNICKNAME;
	this->nickname = nickname;
	return 0;
}

int User::setUsername(const std::string &username)
{
	regex user_regex(R"(^[^\s@]{1,10}$)");
	if (regex_match(username, user_regex) == false)
		return 1;
	this->username = username;
	return 0;
}

int User::setHostname(const std::string &hostname)
{
	regex host_regex(R"(^(?=.{1,255}$)([a-zA-Z0-9]([a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(\.[a-zA-Z0-9]{1,})*)$)");
	if (regex_match(hostname, host_regex) == false)
		return 1;
	this->hostname = hostname;
	return 0;
}

int User::setServername(const std::string &servername)
{
	regex server_regex(R"(^(?=.{1,255}$)([a-zA-Z0-9]([a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(\.[a-zA-Z0-9]{1,})*)$)");
	if (regex_match(servername, server_regex) == false)
		return 1;
	this->servername = servername;
	return 0;
}

int User::setRealname(const std::string &realname)
{
	regex real_regex(R"(^[\x20-\x7E]{1,50}$)");
	if (regex_match(realname, real_regex) == false)
		return 1;
	this->realname = realname;
	return 0;
}

int User::setInfo(const std::string &args)
{
	std::string user, host, server, real;
	istringstream stream(args);
	stream >> user;
	stream >> host;
	stream >> server;
	stream.ignore(2); // skip space and ':'
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

int User::privmsg(const User &recipient, const std::string &message) const
{
	if (message.empty())
		return ERR_NOTEXTTOSEND;
	IO::sendCommand(recipient.fd, {getFullIdentifier(), recipient.nickname, message});
	return 0;
}

int User::privmsg(const Channel &channel, const std::string &message) const
{
	// if channel.getMode... +m or +b mode
	//	return ERR_CANNOTSENDTOCHAN;
	for (const auto &pair : channel.getUserList())
	{
		int ret = privmsg(pair.second, message);
		if (ret != 0)
			return ret;
	}
	return 0;
}

int User::join(Channel &channel)
{
	return join(channel, ""); 
	// if no password is given, try to login with an empty password.
	// If channel is not password protected, it could have an empty password so this works
}

int User::join(Channel &channel, const string &password)
{
	if (password != channel.getPassword())
		return ERR_BADCHANNELKEY;
	if (channel.isInviteOnly())
		return ERR_INVITEONLYCHAN;
	if (channel.getUserLimit() >= channel.getUserList().size())
		return ERR_CHANNELISFULL;
	channel.addUser(username, *this);
	joinedChannels[channel.getChannelName()] = channel;
	return 0;
}

void User::setAuth(const bool status)
{
	isAuth = status;
}

bool User::getAuth() const
{
	return isAuth;
}

int User::part(Channel &channel, const std::string &message) // leaves a channel with a goodbye message
{
	for (const auto &pair : channel.getUserList())
	{
		User u = pair.second;
		if (IO::sendCommand(u.fd, {getFullIdentifier(),
			"PART", "#" + channel.getChannelName() + " :" + message}) < 0)
			return -1;
	}
	joinedChannels.erase(channel.getChannelName());
	channel.removeUser(nickname);
	return 0;
}

int User::quit(const std::string &message) // leaves all joined channels
{
	std::map<string, Channel> channelsCopy = joinedChannels;
	for (const auto &pair : channelsCopy) // iterate the copy so that the original can be modified at the same time
	{
		Channel c = pair.second;
		if (part(c, message) < 0)
			return -1; // send error
	}
	return 0;
}

bool operator==(const User &lhs, const User &rhs) {
	return lhs.getFd() == rhs.getFd();
}