#include "Server.hpp"

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

int	Server::NICK(cmd cmd, User &user) {
	if (cmd.arguments.empty()) {
		return (ERR_NONICKNAMEGIVEN);
	} else if (_nickIsUsed(cmd.arguments)) {
		return (ERR_NICKNAMEINUSE);
	} else if (user.setNickname(cmd.arguments)) {
		return (ERR_ERRONEUSNICKNAME);
	}
	user.setNickIsSet(true);
	sendMessage(RPL_WHOISUSER, cmd, user);

	if (user.getUserIsSet() && !user.getIsRegistered()) {
		user.setIsRegistered(true);
		return (RPL_WELCOME);
	}
	return (0);
}

int	Server::USER(cmd cmd, User &user) {
	parsedArgs userArgs = parseArgs(cmd.arguments, 4, true);

	if (_userIsUsed(userArgs.args[0])) { // add unique number to end so things will work with irssi.
		log(DEBUG, "USER", "Username " + userArgs.args[0] + " is taken. Creating unique username...");
		for (int i = 1;; ++i)
		{
			if (!_userIsUsed(userArgs.args[0] + to_string(i))) {
				userArgs.args[0] += to_string(i);
				break;
			}
		}
	}
	
	if (userArgs.size < 4) {
		return (ERR_NEEDMOREPARAMS);
	} else if (user.setUsername(userArgs.args[0])
		|| user.setHostname(userArgs.args[1])
		|| user.setServername(userArgs.args[2])
		|| user.setRealname(userArgs.trailing)) {
		return ERR_ERRONEUSUSER;
	}

	user.setUserIsSet(true);
	sendMessage(RPL_WHOISUSER, cmd, user);

	if (user.getNickIsSet() && !user.getIsRegistered()) {
		user.setIsRegistered(true);
		return (RPL_WELCOME);
	}
	return (0);
}

int	Server::JOIN(cmd cmd, User &user) {
	if (cmd.arguments.empty()) {
		return (ERR_NEEDMOREPARAMS);
	} else if (cmd.arguments == "0") {
		user.quit(user.getNickname() + " left"); //part all joined channels
		return (0);
	}
	parsedArgs 		joinArgs = parseArgs(cmd.arguments, 2, false);
	vector<string>	channels, keys;

	channels = commaSplit(joinArgs.args[0]);
	if (joinArgs.size == 2) {
		keys = commaSplit(joinArgs.args[1]);
	}

	size_t	keySize = (joinArgs.size == 2) ? keys.size() : 0;
	size_t	channelSize = channels.size();
	if (keySize > channelSize) {
		return (ERR_NEEDMOREPARAMS);
	}

	for (size_t index = 0; index < channelSize; ++index) {
		int		code = 0;
		Channel *channel;

		if (!isValidChannelName(channels[index])) {
			code = ERR_BADCHANMASK;
		} else {
			string keyValue = (index < keySize) ? keys[index] : "";
			channel = this->findChannelByName(channels[index]);
			code = (channel == nullptr) ? 
					createChannel(channel, user, channels[index], keyValue) :
					user.join(*channel, keyValue);
		}

		if (code) {
			cmd.arguments = channels[index];
			return (code);
		}
		sendMessage(RPL_TOPIC, cmd, user, *channel);
		sendMessage(RPL_NAMREPLY, cmd, user, *channel);
		IO::sendString(user.getFd(), ":" + _name + " 366 " + user.getNickname() + " " + channel->getChannelName() + " :End of /NAMES list.");
	}
	return (0);
}

int	Server::PRIVMSG(cmd cmd, User &user) {
	if (cmd.arguments.empty()) {
		return (ERR_NORECIPIENT);
	}

	parsedArgs priArgs = parseArgs(cmd.arguments, 2, true);
	//cout << priArgs.size;
	if (priArgs.size < 2) {
		return (ERR_NOTEXTTOSEND);
	}

	string target = priArgs.args[0];
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
			return(user.privmsg(*targetUser, priArgs.trailing));
		}
	} else {
		Channel *targetChannel = findChannelByName(target);

		if (targetChannel == nullptr) {
			return (ERR_NOSUCHNICK);
		} else {
			return (user.privmsg(*targetChannel, priArgs.trailing));
		}
	}
}

// int	Server::OPER(cmd cmd, User &user) {
// 	if (countWords(cmd.arguments) < 2) {
// 		return (ERR_NEEDMOREPARAMS);
// 	}
// 	return 0;
// }


int	Server::QUIT(cmd cmd, User &user) {
	string message = cmd.arguments;
	if (cmd.arguments.empty())
		message = user.getNickname() + " quit";
	if (user.quit(":" + message) == -1)
		throw runtime_error("QUIT: send error");
	removeUser(user.getFd());
	return 0;
}

int	Server::PART(cmd cmd, User &user) {
	if (cmd.arguments.empty()) {
		return (ERR_NEEDMOREPARAMS);
	}
	parsedArgs partArgs = parseArgs(cmd.arguments, 2, true);
	string message = "";

	if (partArgs.trailing.empty()) {
		message += user.getNickname() + " left";
	} else {
		message += partArgs.trailing;
	}

	vector<string> channelList = commaSplit(partArgs.args[0]);

	for (size_t index = 0; index < channelList.size(); index++) {
		string channelName = channelList[index];
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

int	Server::WHOIS(cmd cmd, User &user) {
	if (cmd.arguments.empty()) {
		return (ERR_NONICKNAMEGIVEN);
	}
	parsedArgs	whoArgs = parseArgs(cmd.arguments, 1, false);
	string		target = whoArgs.args[0];

	if (targetIsUser(target[0])) {
		User *targetUser = findUserByNickName(target);

		if (targetUser == nullptr) {
			return (ERR_NOSUCHNICK);
		} else {
			cmd.arguments = targetUser->getNickname() + " " + targetUser->getUsername() + " " + targetUser->getHostname() + " * :" + targetUser->getRealname();
			sendMessage(RPL_WHOISUSER, cmd, user);
			return (0);
		}
	}
	return (0);
}

