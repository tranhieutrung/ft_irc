
#include "Server.hpp"


std::optional<std::map<string, Channel>::iterator> Server:: findChannel(string channel)
{
	auto it = channels.find(channel);
    if (it != channels.end()) {
        return it;
    }
    return std::nullopt;
}


int	Server::TOPIC(cmd cmd, User &user)

{
	string		res;
	string channel;
	string topic;
	istringstream stream(cmd.arguments);
	string message;

	stream >> channel >> topic;
	if (channel.empty())
	{
		return (ERR_NEEDMOREPARAMS);
	}
	auto itOpt = findChannel(channel);
    if (!itOpt.has_value()) {
        return ERR_NOSUCHCHANNEL;
    }
    std::map<string, Channel>::iterator it = *itOpt;
	if (topic.empty())
	{
		message = it->second.getChannelTopic();
		message += "\r\n";
		if (send(user.getFd(), message.c_str(), message.length(), 0) == -1)
			cerr << "send() error: " << strerror(errno) << endl;
		return (0);
	}
	if (it->second.isTopicRestricted() && !it->second.isOperator(user))
	{
		return (ERR_CHANOPRIVSNEEDED);
	}
	else
	{
		it->second.setChannelTopic(topic);
		message = user.getNickname() + " has set topic to " + topic + "\r\n";
		user.privmsg(channel, message);
	}
	return (0);

}

int	Server::KICK(cmd cmd, User &user)
{
	string channel;
	string target;
	string res;
	istringstream stream(cmd.arguments);

	stream >> channel >> target;
	if (channel.empty() || target.empty())
	{
		return (ERR_NEEDMOREPARAMS);
	}
	auto itOpt = findChannel(channel);
    if (!itOpt.has_value()) {
        return ERR_NOSUCHCHANNEL;
    }

    std::map<string, Channel>::iterator it = *itOpt;
	if (!it->second.isOperator(user))
	{
		return (ERR_CHANOPRIVSNEEDED);
	}
	std::optional<std::map<int, User>::iterator> it2 = it->second.findUserByNickname(target);
	if (!it2)
	{
		return (ERR_NOSUCHNICK);
	}
    User &targetUser = it2.value()->second;
    targetUser.part(it->second, target + " was kicked by " + user.getNickname());
	return (0);
}

int	Server::MODE(cmd cmd, User &user)
{
	string mode;
	string channel;
	string extra;
	string		res;
	log_level	type;
	istringstream stream(cmd.arguments);

	stream >> channel >> mode >> extra;

	if (channel.empty())
    {
        return (ERR_NEEDMOREPARAMS);
    }

	auto itOpt = findChannel(channel);
    if (!itOpt.has_value()) {
		return ERR_NOSUCHCHANNEL;
    }

	std::map<string, Channel>::iterator it = *itOpt;
	Channel c = it->second;

	if (mode.empty())
	{
		std::string modes = "+";
		if (c.isInviteOnly())
			modes += "i";
		if (c.isTopicRestricted())
			modes += "t";
		if (!c.getPassword().empty())
			modes += "k";
		IO::sendString(user.getFd(), ":" + _name + " 324 " + user.getNickname() + " " + c.getChannelName() + (modes == "+" ? "" : " " + modes));
		return 0;
	}

	if (!it->second.isOperator(user))
	{
		return (ERR_CHANOPRIVSNEEDED);
	}
	if (mode == "-i")
	{
		it->second.setInviteOnly(false);
        res = "Switched Invite only off";
        type = INFO;
        log(type, cmd.command, res);
        return (0);
	}
	if (mode == "+i")
	{
		it->second.setInviteOnly(true);
        res = "Switched Invite only on";
        type = INFO;
        log(type, cmd.command, res);
        return (0);
	}
	if (mode == "-t")
	{
		if (!it->second.isTopicRestricted())
			it->second.setTopicRestriction(true);
		else
			it->second.setTopicRestriction(false);
        res = "Switched topic restriction off";
        type = INFO;
        log(type, cmd.command, res);
        return (0);
	}
	if (mode == "+t")
	{
		it->second.setTopicRestriction(true);
        res = "Switched topic restriction on";
        type = INFO;
        log(type, cmd.command, res);
        return (0);
	}
	if (mode == "-k")
	{
        it->second.setPassword("");
        res = "removed password";
        type = INFO;
        log(type, cmd.command, res);
        return (0);
	}
	if (mode == "+k")
	{
		if (extra.empty())
		{
            return (ERR_NEEDMOREPARAMS);
		}
        else
		{
            it->second.setPassword(extra);
		}
        res = "Switched password";
        type = INFO;
        log(type, cmd.command, res);
        return (0);
	}
	if (mode == "-o")
	{

		if (it->second.findUserByNickname(extra))
		{
            const User *opp =  getUser(extra);
			it->second.removeOperator(*opp);
		}
		else{
			return (ERR_NOSUCHNICK);
		}
        res = "removed operator";
        type = INFO;
        log(type, cmd.command, res);
        return (0);
	}
	if (mode == "+o")
	{

		if (it->second.findUserByNickname(extra))
		{
            const User *opp =  getUser(extra);
			it->second.addOperator(*opp);
		}
		else{
			return (ERR_NOSUCHNICK);
		}
        res = "Added operator";
        type = INFO;
        log(type, cmd.command, res);
        return (0);
	}
	if (mode == "-l")
	{
		it->second.setUserLimit(999);
        res = "removed Userlimit";
        type = INFO;
        log(type, cmd.command, res);
        return (0);
	}
	if (mode == "+l")
	{
		if (extra.empty())
		{
			return (ERR_NEEDMOREPARAMS);
		}
		else
		{
			it->second.setUserLimit(stoi(extra));
		}
        res = "Set Userlimit";
        type = INFO;
        log(type, cmd.command, res);
        return (0);
	}
	else
	{
		return (ERR_UNKNOWNMODE);
	}
}


int	Server::INVITE(cmd cmd, User &user)
{
	string target;
	string channel;
	string		res;
	istringstream stream(cmd.arguments);
	string message;
	string message2;

	stream >> channel >> target;

	if (channel.empty() || target.empty())
    {
        return (ERR_NEEDMOREPARAMS);
    }
	auto itOpt = findChannel(channel);
    if (!itOpt.has_value()) {
        return ERR_NOSUCHCHANNEL;
    }

    std::map<string, Channel>::iterator it = *itOpt;
	if (!it->second.isOperator(user))
	{
		return (ERR_CHANOPRIVSNEEDED);
	}
	const User *invited = getUser(target);
	if (!invited)
	{
		return (ERR_NOSUCHNICK);
	}
	message =  "Invited " + target + "\r\n";

	if (send(user.getFd(), message.c_str(), message.length(), 0) == -1)
		cerr << "send() error: " << strerror(errno) << endl;
	message2= "You have been invited by " + user.getNickname() + "to channel " + channel + "\r\n";
	if (send(invited->getFd(), message2.c_str(), message2.length(), 0) == -1)
		cerr << "send() error: " << strerror(errno) << endl;
	it->second.addUser(invited->getFd(), *invited);
	return (0);

}
