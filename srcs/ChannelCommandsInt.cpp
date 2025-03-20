
#include "Server.hpp"



std::map<string, Channel>::iterator Server:: findChannel (string channel)
{
	return (channels.find(channel));
}


int	Server::_processTOPIC(cmd cmd, User &user) 

{	
	string		res;
	log_level	type;
	string channel;
	string topic;
	istringstream stream(cmd.arguments);

	stream >> channel >> topic;
	if (channel.empty())
	{
		return (ERR_NEEDMOREPARAMS);
	}
	std::map<string, Channel>::iterator it = findChannel(channel);
	if (it != channels.end())
	{
		return (ERR_NOSUCHCHANNEL);
	}
	if (!topic.empty())
	{
		return(it->second.getChannelTopic());
	}
	if (it->second.isTopicRestricted() && !user.getIsOperator())
	{
		return (ERR_CHANOPRIVSNEEDED);
	}
	else 
	{
		it->second.setChannelTopic(topic);
	}
	res = "Changed topic for channel";
	type = INFO;
	log(type, cmd.command, res);
	return (0);
	
}



int	Server::_processKICK(cmd cmd, User &user) 
{
	string channel;
	string target;
	string		res;
	log_level	type;
	istringstream stream(cmd.arguments);

	stream >> channel >> target;
	if (channel.empty() || target.empty())
	{
		return (ERR_NEEDMOREPARAMS);
	}
	if (!user.getIsOperator())
	{
		return (ERR_CHANOPRIVSNEEDED);
	}
	std::map<string, Channel>::iterator it = findChannel(channel);
	if (it != channels.end())
	{
		return (ERR_NOSUCHCHANNEL);
	}
	std::optional<std::map<string, User>::iterator> it2 = it->second.findUser(target);
	if (!it2)
	{
		return (ERR_NOSUCHNICK);
	}
	it->second.removeUser(user.getUsername());
	res = "Kicked user from channel";
	type = INFO;
	log(type, cmd.command, res);
	return (0);
}

int	Server::_processMODE(cmd cmd, User &user) 
{
	string mode;
	string channel;
	string extra;
	string		res;
	log_level	type;
	istringstream stream(cmd.arguments);
	
	stream >> channel >> mode >> extra;

	if (channel.empty() || mode.empty())
    {
        return (ERR_NEEDMOREPARAMS);
    }
	if (!user.getIsOperator())
	{
		return (ERR_CHANOPRIVSNEEDED);
	}
	std::map<string, Channel>::iterator it = findChannel(channel);
	if (it != channels.end())
	{
		return (ERR_NOSUCHCHANNEL);
	}
	if (mode == "-i")
	{
		if (!it->second.isInviteOnly())
			it->second.setInviteOnly(true);
		else
			it->second.setInviteOnly(false);
        res = "Switched Invite only";
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
        res = "Switched topic restriction only";
        type = INFO;
        log(type, cmd.command, res);
        return (0);
	}
	if (mode == "-k")
	{
		if (extra.empty())
		{
            it->second.setPassword(nullptr);
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
		
		if (it->second.findUser(extra))
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
		if (extra.empty())
		{
			it->second.setUserLimit(999);
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
	

int	Server::_processINVITE(cmd cmd, User &user)
{
	string target;
	string channel;
	string		res;
	log_level	type;
	istringstream stream(cmd.arguments);

	stream >> channel >> target;
	
	if (channel.empty() || target.empty())
    {
        return (ERR_NEEDMOREPARAMS);
    }
	if (!user.getIsOperator())
	{
		return (ERR_CHANOPRIVSNEEDED);
	}
	std::map<string, Channel>::iterator it = findChannel(target);
	if (it != channels.end())
	{
		return (ERR_NOSUCHCHANNEL);
	}
	const User *invited = getUser(target);
	if (!invited)
	{
		return (ERR_NOSUCHNICK);
	}
	//sendInvite();
	it->second.addUser(invited->getNickname(), *invited);
	res = "Invite send to user";
	type = INFO;
	log(type, cmd.command, res);
	return (0);

}