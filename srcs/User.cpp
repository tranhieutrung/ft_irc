#include "../includes/User.hpp"
#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"
#include "../includes/ErrorCodes.hpp"
#include "../includes/IO.hpp"
#include <sstream>
#include <regex>
#include <vector>
#include <optional>

User::User() :
	nickname("Unknown"),
	username(""),
	hostname("localhost"),
	servername(""),
	realname(""),
	fd(-1),
	isOperator(false),
	isAuth(false),
	nickIsSet(false),
	userIsSet(false),
	isRegistered(false) {}

User::User(const int fd) :
	nickname("User" + to_string(fd -3)),
	username(""),
	hostname("localhost"),
	servername(""),
	realname(""),
	fd(fd),
	isOperator(false),
	isAuth(false),
	nickIsSet(false),
	userIsSet(false),
	isRegistered(false) {}

User::User(const User &other) :
	nickname(other.nickname),
	username(other.username),
	hostname(other.hostname),
	servername(other.servername),
	realname(other.realname),
	fd(other.fd),
	isOperator(other.isOperator),
	isAuth(other.isAuth),
	nickIsSet(other.nickIsSet),
	userIsSet(other.userIsSet),
	isRegistered(other.isRegistered) {}

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
	nickIsSet = other.nickIsSet;
	userIsSet = other.userIsSet;
	isRegistered = other.isRegistered;
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
	return 0;
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
	// this->userIsSet = true;
	return 0;
}

string User::getFullIdentifier() const
{
	return ":" + nickname + "!" + username + "@" + hostname;
}

int User::privmsg(const User &recipient, const std::string &message) const
{
	if (message.empty())
		return ERR_NOTEXTTOSEND;
	IO::sendCommand(recipient.fd, {getFullIdentifier(), "PRIVMSG", recipient.nickname + " " + message});
	return 0;
}

int User::privmsg(const Channel &channel, const std::string &message) const
{
	if(!channel.findUser(fd))
		return ERR_NOTONCHANNEL;
	for (const auto &pair : channel.getUserList())
	{
		if (pair.second.getFd() == fd)
			continue;
		int ret = IO::sendCommand(pair.second.getFd(), {getFullIdentifier(), "PRIVMSG", channel.getChannelName() + " " + message});
		if (ret < 0)
			return ret;
	}
	return 0;
}

int User::join(Channel &channel)
{
	// if no password is given, try to login with an empty password.
	// If channel is not password protected, it could have an empty password so this works
	return join(channel, ""); 
}

int User::join(Channel &channel, const string &password)
{
	if (password != channel.getPassword())
		return ERR_BADCHANNELKEY;
	if (channel.isInviteOnly())
		return ERR_INVITEONLYCHAN;
	if (channel.getUserLimit() <= channel.getUserList().size())
		return ERR_CHANNELISFULL;
	channel.addUser(fd, *this);
	if (IO::sendCommandAll(channel.getUserList(), {getFullIdentifier(), "JOIN", channel.getChannelName()}) < 0)
		throw runtime_error("send failed");
	return 0;
}

int User::part(Channel &channel, const std::string &message) // leaves a channel with a goodbye message
{
	if(!channel.findUser(fd))
		return ERR_NOTONCHANNEL;
	for (const auto &pair : channel.getUserList())
	{
		User u = pair.second;
		if (IO::sendCommand(u.fd, {getFullIdentifier(),
			"PART", channel.getChannelName() + (message.empty() ? "" : " " + message)}) < 0)
			return -1;
	}
	channel.removeUser(fd);
	return 0;
}

bool operator==(const User &lhs, const User &rhs) {
	return lhs.getFd() == rhs.getFd();
}

bool operator!=(const User &lhs, const User &rhs) {
	return lhs.getFd() != rhs.getFd();
}
