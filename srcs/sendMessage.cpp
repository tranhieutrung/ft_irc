#include "../includes/Server.hpp"

string	Server::createMessage(int code, cmd cmd, User &user) {
	string message;

	message = ":" + this->_name + " ";
	if (code < 10) {
		message += "00";
	}
	message += to_string(code) + " " + user.getNickname() + " ";
	
	if (code == RPL_WELCOME) {
		message += ":Welcome to the Internet Relay Network " + user.getFullIdentifier();
	} else if (code == ERR_NEEDMOREPARAMS) {
		message += cmd.command + " :Not enough parameters";
	} else if (code == ERR_PASSWDMISMATCH) {
		message += ":Password incorrect";
	} else if (code == ERR_ALREADYREGISTRED) {
		message += ":Unauthorized command (already registered)";
	} else if (code == ERR_NOTONCHANNEL) {
		message += ":You're not on the channel" ;
	}  else if (code == ERR_USERONCHANNEL) {
		message += ":User already in the channel" ;
	} else if (code == ERR_NOLOGIN) {
		message += user.getUsername() + " :User not logged in";
	} else if (code == ERR_NONICKNAMEGIVEN) {
		message += ":No nickname given";
	} else if (code == ERR_NICKNAMEINUSE) {
		message += cmd.arguments + " :Nickname is already in use";
	} else if (code == ERR_ERRONEUSNICKNAME) {
		message += cmd.arguments + " :Erroneous nickname";
	} else if (code == ERR_UNKNOWNCOMMAND) {
		message += cmd.command + " :Unknown command";
	} else if (code == ERR_NOTREGISTERED) {
		message += ":You have not registered";
	} else if (code == ERR_NOORIGIN) {
		message += ":No origin specified";
	} else if (code == ERR_NOSUCHSERVER) {
		message += cmd.arguments + " :No such server";
	} else if (code == ERR_INVITEONLYCHAN) {
		message += cmd.arguments + " :Cannot join channel (+i)";
	} else if (code == ERR_CHANNELISFULL) {
		message += cmd.arguments + " :Cannot join channel (+l)";
	} else if (code == ERR_BADCHANNELKEY) {
		message += cmd.arguments + " :Cannot join channel (+k)";
	} else if (code == ERR_BADCHANMASK) {
		message += cmd.arguments + " :Bad Channel Mask";
	} else if (code == ERR_UNKNOWNMODE) {
		message += cmd.arguments + " :Unknown mode";
	} else if (code == ERR_CHANOPRIVSNEEDED) {
		message += cmd.arguments + " :You're not channel operator";
	} else if (code == ERR_NOSUCHCHANNEL) {
		message += cmd.arguments + " :No such channel";
	} else if (code == ERR_NOSUCHNICK) {
		message += cmd.arguments + " :No such nick/channel";
	} else if (code == ERR_NORECIPIENT) {
		message += ":No recipient given";
	} else if (code == ERR_NOTEXTTOSEND) {
		message += ":No text to send";
	} else if (code == ERR_NOTOPLEVEL) {
		message += cmd.arguments + " :No toplevel domain specified";
	} else if (code == ERR_WILDTOPLEVEL) {
		message += cmd.arguments + " :Wildcard in toplevel domain";
	} else if (code == ERR_CANNOTSENDTOCHAN) {
		message += cmd.arguments + " :Cannot send to channel"; 
	} else if (code == ERR_TOOMANYTARGETS) {
		message += cmd.arguments + " :Too many targets";
	} else if (code == RPL_WHOISUSER) {
		message += cmd.arguments;
	} else if (code == RPL_PONG) {
		message = ":" + this->_name + " PONG "+ this->_name;
	} else if (code == ERR_ERRONEUSUSER) {
		message += cmd.arguments + " :Erroneous format";
	//last
	} else {
		message += cmd.command + " " + cmd.arguments;
	}
	message += "\r\n";

	return (message);
}

void Server::sendMessage(int code, cmd cmd, User &user) {
	if (!code)
		return ;
	string message = createMessage(code, cmd, user);
	if (code && IO::sendString(user.getFd(), message) == -1)
		cerr << "send() error: " << strerror(errno) << endl;
}

std::string Server::createMessage(int code, cmd cmd, User &user, Channel &channel) {
    std::string message;

    (void)cmd;
    message = ":" + this->_name + " " + std::to_string(code) + " " + user.getNickname() + " ";

    if (code == RPL_TOPIC) {
        message += channel.getChannelName() + " :" + channel.getChannelTopic();
    } else if (code == RPL_NAMREPLY) {
        message += "= " + channel.getChannelName() + " :";
        for (auto &it : channel.getUserList()) {
            bool isOp = channel.getOperators().count(it.second->getFd()) > 0; 
            message += (isOp) ? "@" : "";
            message += it.second->getNickname() + " ";
        }
        if (!message.empty() && message.back() == ' ')
            message.pop_back();
    }
    message += "\r\n";
    return message;
}

void Server::sendMessage(int code, cmd cmd, User &user, Channel &channel) {
	if (!code)
		return ;
	string message = createMessage(code, cmd, user, channel);
	if (code && IO::sendString(user.getFd(), message) == -1)
		cerr << "send() error: " << strerror(errno) << endl;
}
