#include "Server.hpp"

int countWords(const 	string &s) {
	size_t pos = s.find(":");
		string beforeColon = (pos != 	string::npos) ? s.substr(0, pos) : s;
	
		stringstream ss(beforeColon);
		string word;
	int count = 0;

	while (ss >> word) {
		count++;
	}

	if (pos != 	string::npos) {
		count++;
	}

	return count;
}

vector<string> commaSplit(string str) {
	vector<string> result;

	if (str.empty()) {
		return result;
	}

	istringstream ss(str);
	string word;
	while (getline(ss, word, ',')) {
		result.push_back(word);
	}
	return (result);
}

int	Server::PING(cmd cmd, User &user) {
	(void)user;
	if (cmd.arguments.empty()) {
		return (ERR_NOORIGIN);
	} else if (cmd.arguments != this->_name) {
		return (ERR_NOSUCHSERVER);
	} else {
		return (0);
	}
}

int	Server::PONG(cmd cmd, User &user) {
	if (cmd.arguments.empty()) {
		return (ERR_NOORIGIN);
	} else if (cmd.arguments != user.getFullIdentifier()) {
		return (ERR_NOSUCHSERVER);
	} else {
		return (0);
	}
}

/** Numeric Replies:
* ERR_NEEDMOREPARAMS		ERR_ALREADYREGISTRED
*/
int	Server::PASS(cmd cmd, User &user) {
	cout << "[" << cmd.arguments << "]" <<endl;
	if (cmd.arguments.empty()) {
		return (ERR_NEEDMOREPARAMS);
	} else if (cmd.arguments != this->_password) {
		return (ERR_PASSWDMISMATCH);
	} else if (user.getAuth()) {
		return (ERR_ALREADYREGISTRED);
	} else {
		user.setAuth(true);
		return (0);
	}
}

bool	Server::_nickIsUsed(string nick) {
	for (auto &[fd, user] : this->users) {
		if (user.getNickname() == nick) {
			return (true);
		}
	}
	return (false);
}

bool	Server::_userIsUsed(string username) {
	for (auto &[fd, user] : this->users) {
		if (user.getUsername() == username) {
			return (true);
		}
	}
	return (false);
}

/** Numeric Replies:
* ERR_NONICKNAMEGIVEN		ERR_ERRONEUSNICKNAME
* ERR_NICKNAMEINUSE			ERR_NICKCOLLISION
* ERR_UNAVAILRESOURCE		ERR_RESTRICTED
*/
int	Server::NICK(cmd cmd, User &user) {
	if (cmd.arguments.empty()) {
		return (ERR_NONICKNAMEGIVEN);
	} else if (_nickIsUsed(cmd.arguments)) {
		return (ERR_NICKNAMEINUSE);
	} else if (user.setNickname(cmd.arguments)) {
		return (ERR_ERRONEUSNICKNAME);
	}
	user.setNickIsSet(true);
	if (user.getUserIsSet() && !user.getIsRegistered()) {
		user.setIsRegistered(true);
		return (RPL_WELCOME);
	}
	return (0);
}

/** Numeric Replies:
* ERR_NEEDMOREPARAMS		ERR_ALREADYREGISTRED
*/
int	Server::USER(cmd cmd, User &user) {
	if (countWords(cmd.arguments) < 4) {
		return (ERR_NEEDMOREPARAMS);
	// } else if (user.getUserIsSet()) {
	// 	return (ERR_ALREADYREGISTRED); //only setup user to register
	}
	user.setInfo(cmd.arguments);
	user.setUserIsSet(true);
	if (user.getNickIsSet() && !user.getIsRegistered()) {
		user.setIsRegistered(true);
		return (RPL_WELCOME);
	}
	return (0);
}


//user create and join a new channel
int Server::createChannel(User user, string channelName, string key) {
	Channel channel(channelName); //should have a channel(name, password)
	channel.setPassword(key);
	channel.addUser(user.getUsername(), user);
	channel.addOperator(user.getNickname());

	this->channels.emplace(channelName, channel);
	user.join(channel, key);
	//if creation failed, return error code
	return (0);
}

/*
*  Channels names are strings (beginning with a '&', '#', '+' or '!'
   character) of length up to fifty (50) characters.  Apart from the
   requirement that the first character is either '&', '#', '+' or '!',
   the only restriction on a channel name is that it SHALL NOT contain
   any spaces (' '), a control G (^G or ASCII 7), a comma (',').  Space
   is used as parameter separator and command is used as a list item
   separator by the protocol).  A colon (':') can also be used as a
   delimiter for the channel mask.  Channel names are case insensitive.
*/
bool isValidChannelName(const string& channelName) {
	if (channelName.empty() || channelName.size() > 50)
		return (false);
    regex channelRegex(R"(^[&#+!][^[:space:],^G,]*$)");
    return regex_match(channelName, channelRegex);
}

/** Numeric Replies:
* ERR_NEEDMOREPARAMS -		ERR_BANNEDFROMCHAN
* ERR_INVITEONLYCHAN -		ERR_BADCHANNELKEY -
* ERR_CHANNELISFULL	- 		ERR_BADCHANMASK -
* ERR_NOSUCHCHANNEL			ERR_TOOMANYCHANNELS
* ERR_TOOMANYTARGETS		ERR_UNAVAILRESOURCE
* RPL_TOPIC
*/
int	Server::JOIN(cmd cmd, User &user) {

	if (cmd.arguments.empty()) {
		return (ERR_NEEDMOREPARAMS);
	} else if (cmd.arguments == "0") {
		user.quit(user.getNickname() + " left"); //part all joined channels
	}

	// Parsing:
	istringstream ss(cmd.arguments);
	string channels_str, keys_str;

	// Separate channel and key string:
	getline(ss, channels_str, ' ');
	getline(ss, keys_str);

	// Comma split:
	vector<string> channels = commaSplit(channels_str);
	vector<string> keys = commaSplit(keys_str);

	if (keys.size() > channels.size()) {
		return (ERR_NEEDMOREPARAMS);
	}
	// crate pairs:
	for (size_t index = 0; index < channels.size(); ++index) {
		if (!isValidChannelName(channels[index])) {
			return (ERR_BADCHANMASK);
		}
		string keyValue = "";

		if (index < keys.size()) {
			keyValue = keys[index];
		}

		Channel *channel = this->findChannelByName(channels[index]);
		int		code = 0;

		if (channel == nullptr) {
			code = createChannel(user, channels[index], keyValue);
		} else {
			code = user.join(*channel, keyValue);
		}

		if (code)
			return (code);
	}

	return (0);
}

int	Server::PRIVMSG(cmd cmd, User &user) {
(void)cmd;
(void) user;
return 0;
}

int	Server::OPER(cmd cmd, User &user) {
(void)cmd;
(void) user;
return 0;
}


int	Server::QUIT(cmd cmd, User &user) {
	if (user.quit(cmd.arguments) == -1) {
		cerr << "Sending messages failes" <<endl;
	}
	return 0;
}

bool isJoinedChannel(User &user, Channel &channel) {
	for (const auto &it : channel.getUserList()) {
		if (it.first == user.getUsername()) {
			return (true);
		}
	}
	return (false);
}

int	Server::PART(cmd cmd, User &user) {
	if (cmd.arguments.empty())
		return (ERR_NEEDMOREPARAMS);

	istringstream ss(cmd.arguments);
	string channelList;
	string	message;
	getline(ss, channelList, ':');
	getline(ss, message);

	istringstream channel_ss(channelList);
	string channelName;

	while (channel_ss >> channelName) {
		Channel *channel = this->findChannelByName(channelName);
		if (channel == nullptr) {
			return (ERR_NOSUCHCHANNEL);
		} else if (!isJoinedChannel(user, *channel)) {
			return (ERR_NOTONCHANNEL);
		} else if (user.part(*channel, message) == -1) {
			cerr << "Sending messages failes" <<endl;
		}
	}
	return 0;
}

// channel commands:

int	Server::KICK(cmd cmd, User &user) {
(void)cmd;
(void) user;
return 0;
}

int	Server::INVITE(cmd cmd, User &user) {
(void)cmd;
(void) user;
return 0;
}

int	Server::TOPIC(cmd cmd, User &user) {
(void)cmd;
(void) user;
return 0;
}

int	Server::MODE(cmd cmd, User &user) {
(void)cmd;
(void) user;
return 0;
}
