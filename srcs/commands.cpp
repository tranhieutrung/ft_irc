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
	vector<string>	channels = commaSplit(joinArgs.args[0]);
	vector<string>	keys = commaSplit(joinArgs.args[1]);

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
			cmd.arguments = channels[index];
			return (code);
		} else {
			if (channel == nullptr) {
				channel = findChannelByName(channels[index]);
			}
			sendMessage(RPL_TOPIC, cmd, user, *channel);
			sendMessage(RPL_NAMREPLY, cmd, user, *channel);
		}
	}
	return (0);
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
	}

	parsedArgs priArgs = parseArgs(cmd.arguments, 2, true);
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
			return(user.privmsg(*targetUser, priArgs.args[1]));
		}
	} else {
		Channel *targetChannel = findChannelByName(target);

		if (targetChannel == nullptr) {
			return (ERR_NOSUCHNICK);
		} else {
			return (user.privmsg(*targetChannel, priArgs.args[1]));
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
	string message = ":" + user.getFullIdentifier() + " QUIT :";

	if (cmd.arguments.empty()) {
		message += user.getNickname() + " quit";
	} else {
		parsedArgs quitArgs = parseArgs(cmd.arguments, 1, true);
		message += quitArgs.trailing;
	}
	if (user.quit(message) == -1) {
		cerr << "Sending messages failes" <<endl;
		return (-1);
	}
	return 0;
}

int	Server::PART(cmd cmd, User &user) {
	if (cmd.arguments.empty()) {
		return (ERR_NEEDMOREPARAMS);
	}
	parsedArgs partArgs = parseArgs(cmd.arguments, 2, true);
	string message = ":" + user.getFullIdentifier() + " PART :";

	if (partArgs.trailing.empty()) {
		message += user.getNickname() + " left";
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

/*
ERR_NOSUCHSERVER	ERR_NONICKNAMEGIVEN
RPL_WHOISUSER		RPL_WHOISCHANNELS
RPL_WHOISCHANNELS	RPL_WHOISSERVER
RPL_AWAY			RPL_WHOISOPERATOR
RPL_WHOISIDLE		ERR_NOSUCHNICK
RPL_ENDOFWHOIS
*/
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

