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

vector<pair<string, string>> parseChannels(const string& arguments) {
	vector<pair<string, string>> result;
	
	istringstream ss(arguments);
	string channels_str, keys_str;

	// Separate channel and key string:
	getline(ss, channels_str, ' ');
	if (!getline(ss, keys_str)) {
		keys_str = "";
	}

	// Comma split:
	vector<string> channels = commaSplit(channels_str);
	vector<string> keys = commaSplit(keys_str);

	// crate pairs:
	for (size_t i = 0; i < channels.size(); ++i) {
		string key_value = (i < keys.size()) ? keys[i] : "";
		result.emplace_back(channels[i], key_value);
	}

	return result;
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
	user.setUserIsSet(true);
	if (user.getNickIsSet() && !user.getIsRegistered()) {
		user.setIsRegistered(true);
		return (RPL_WELCOME);
	}
	return (0);
}

bool Server::existChannel(string Name) {
	for (auto &[str, channel] : this->channels) {
		if (channel.getChannelName() == Name) {
			return (true);
		}
	}
	return (false);
}
//user create and join a new channel
int Server::createChannel(User user, string channelName, string key) {
	(void)user;
	(void)channelName;
	(void)key;
	return (0);
}

/** Numeric Replies:
* ERR_NEEDMOREPARAMS		ERR_BANNEDFROMCHAN
* ERR_INVITEONLYCHAN		ERR_BADCHANNELKEY
* ERR_CHANNELISFULL			ERR_BADCHANMASK
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
	vector<pair<string, string>> channelPairs = parseChannels(cmd.arguments);
	for (pair<string, string> channel : channelPairs) {
		if (!existChannel(channel.first)) {
			return (createChannel(user, channel.first, channel.second));
		}
		//check too many channels: ERR_TOOMANYCHANNELS
		//check  invited channel: ERR_INVITEONLYCHAN
		//check use banned: ERR_BANNEDFROMCHAN
		//check full chanel: ERR_CHANNELISFULL
		//check channel key: ERR_BADCHANNELKEY
		//check other: ERR_BADCHANMASK, ERR_UNAVAILRESOURCE, ERR_NOSUCHCHANNEL, ERR_TOOMANYTARGETS

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
(void)cmd;
(void) user;
return 0;
}

int	Server::PART(cmd cmd, User &user) {
(void)cmd;
(void) user;
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
