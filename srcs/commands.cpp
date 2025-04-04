#include "Server.hpp"

int countWords(const 	string &s) {
	size_t pos = s.find(":");
	string beforeColon = (pos != 	string::npos) ? s.substr(0, pos) : s;
	
	istringstream ss(beforeColon);
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
		return (RPL_PONG);
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
	// cout << "[" << cmd.arguments << "]" <<endl;
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
	for (auto &it : this->users) {
		if (it.second.getUsername() == username) {
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
	sendMessage(RPL_WHOISUSER, cmd, user);
	return (0);
}

/** Numeric Replies:
* ERR_NEEDMOREPARAMS		ERR_ALREADYREGISTRED
*/
int	Server::USER(cmd cmd, User &user) {
	std::string username, hostname, servername, realname;
	istringstream stream(cmd.arguments);
	stream >> username;
	stream >> hostname;
	stream >> servername;
	stream.ignore(2); // skip space and ':'
	getline(stream, realname);

	while (_userIsUsed(username))
	{
		username += "1";
	}

	if (user.setUsername(username) == 1 // these will return 1 if empty
		|| user.setHostname(hostname) == 1
		|| user.setServername(servername) == 1
		|| user.setRealname(realname) == 1)
		return ERR_NEEDMOREPARAMS;

	sendMessage(RPL_WHOISUSER, cmd, user);
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
	int code = user.join(channel, key);
	if (!code) {
		channel.addOperator(user);
		this->channels.emplace(channelName, channel);
	}
	return (code);
}

/*
* Channels names are strings (beginning with a '&', '#', '+' or '!'
 character) of length up to fifty (50) characters. Apart from the
 requirement that the first character is either '&', '#', '+' or '!',
 the only restriction on a channel name is that it SHALL NOT contain
 any spaces (' '), a control G (^G or ASCII 7), a comma (','). Space
 is used as parameter separator and command is used as a list item
 separator by the protocol). A colon (':') can also be used as a
 delimiter for the channel mask. Channel names are case insensitive.
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
* RPL_TOPIC					RPL_NAMREPLY
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

	for (size_t index = 0; index < channels.size(); ++index) {
		int		code = 0;
		Channel *channel;

		if (!isValidChannelName(channels[index])) {
			code = ERR_BADCHANMASK;
		} else {
			string keyValue = (index < keys.size()) ? keys[index] : "";
			channel = this->findChannelByName(channels[index]);
			code = (channel == nullptr) ? 
					createChannel(user, channels[index], keyValue) : 
					user.join(*channel, keyValue);
		}

		if (code) {
			cmd.arguments = channels[index]; //update to print proper message
			return (code);
		} else {
			if (channel == nullptr) {
				channel = findChannelByName(channels[index]);
			}
			sendMessage(0, cmd, user);
			sendMessage(RPL_TOPIC, cmd, user, *channel);
			sendMessage(RPL_NAMREPLY, cmd, user, *channel);
		}
	}
	return (0);
}

bool matchesWildcard(const string &pattern, const string &target) {
	string regexPattern = "^" + regex_replace(pattern, regex("\\*"), ".*") + "$";
	regex re(regexPattern);
	return regex_match(target, re);
}

bool targetIsUser(char c) {
	if (c == '#' || c == '&' || c == '+' || c == '!')
		return (false);
	return (true);
}

/*
* Numeric Replies:
		ERR_NORECIPIENT				ERR_NOTEXTTOSEND
		ERR_CANNOTSENDTOCHAN		ERR_NOTOPLEVEL
		ERR_WILDTOPLEVEL			ERR_TOOMANYTARGETS
		ERR_NOSUCHNICK
		RPL_AWAY
*/
int	Server::PRIVMSG(cmd cmd, User &user) {
	if (cmd.arguments.empty()) {
		return (ERR_NORECIPIENT);
	} else if (countWords(cmd.arguments) < 2) {
		return (ERR_NEEDMOREPARAMS);
	}

	istringstream 	argSS(cmd.arguments);
	string			target, message;

	getline(argSS, target, ' ');
	getline(argSS, message);

	if (message.empty()) {
		argSS.clear(); // clear any error status
		argSS.seekg(0); // move the pointer to the beginning
		argSS >> target >> message; // If there is more than one space, it will skip over the remaining words.
	}
	if (message.empty())
		return (ERR_NOTEXTTOSEND);

	cmd.arguments = target;
	if (target.find(',') != string::npos) {
		return (ERR_TOOMANYTARGETS);
	} else if (targetIsUser(target[0])) {
		string nickName;
		size_t pos = target.find('!');
		if (pos != string::npos) {
			nickName = target.substr(0, pos);
		} else {
			nickName = target;
		}
		User *targetUser = findUserByNickName(nickName);
		if (targetUser == nullptr) {
			return (ERR_NOSUCHNICK);
		} else {
			return(user.privmsg(*targetUser, message));
		}
	} else {
		Channel *targetChannel = findChannelByName(target);

		if (targetChannel == nullptr) {
			return (ERR_NOSUCHNICK);
		} else {
			return (user.privmsg(*targetChannel, message));
		}
	}
}

// int	Server::OPER(cmd cmd, User &user) {
// (void)cmd;
// (void) user;
// return 0;
// }


int	Server::QUIT(cmd cmd, User &user) {
	size_t pos = cmd.arguments.find(':');
	string message;
	string temp;

	message = ":" + user.getFullIdentifier() + " QUIT :";
	if (pos != string::npos) {
		temp = cmd.arguments.substr(pos + 1);
	} else {
		istringstream ss(cmd.arguments);
		ss >> temp;
	}

	if (temp.empty()) {
		temp = "Client quit";
	}
	message += temp;
	if (user.quit(message) == -1) {
		cerr << "Sending messages failes" <<endl;
		return (-1);
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

	istringstream 	argSS(cmd.arguments);
	string			channelList;
	string			message;

	getline(argSS, channelList, ':');
	getline(argSS, message);

	if (message.empty()) {
		argSS.clear(); // clear any error status
		argSS.seekg(0); // move the pointer to the beginning
		argSS >> channelList >> message; // If there is more than one space, it will skip over the remaining words.
	}

	istringstream channel_ss(channelList);
	string channelName;

	while (getline(channel_ss, channelName, ',')) {
		if (channelName.empty())
			continue;
		Channel *channel = this->findChannelByName(channelName);
		if (channel == nullptr) {
			return (ERR_NOSUCHCHANNEL);
		} else if (!isJoinedChannel(user, *channel)) {
			return (ERR_NOTONCHANNEL);
		} else if (user.part(*channel, message) == -1) {
			cerr << "Sending messages failes" <<endl;
			return (-1);
		}
	}
	return 0;
}
