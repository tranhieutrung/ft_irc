
#include "Server.hpp"



std::map<string, Channel>::iterator Server:: findChannel (string channel)
{
	return (channels.find(channel));
}


string	Server::_processTOPIC(cmd cmd, User &user) 

{	
	string		res;
	log_level	type;
	string channel;
	string topic;
	istringstream stream(cmd.arguments);

	stream >> channel >> topic;

	if (channel.empty())
	{
		res = "Invalid number of arguments";
		type = ERROR;
		log(type, cmd.command, res);
		return (res);
	}
	std::map<string, Channel>::iterator it = findChannel(channel);
	if (it != channels.end())
	{
		res = "Channel not found";
		type = ERROR;
		log(type, cmd.command, res);
		return (res);
	}
	if (!topic.empty())
	{
		return(it->second.getChannelTopic());
	}
	if (it->second.isTopicRestricted() && !user.getIsOperator())
	{
		res = "No permission for this command";
		type = ERROR;
		log(type, cmd.command, res);
		return (res);
	}
	else 
	{
		it->second.setChannelTopic(topic);
	}
	res = "Changed topic for channel";
	type = INFO;
	log(type, cmd.command, res);
	return (res);
	
}



string	Server::_processKICK(cmd cmd, User &user) 
{
	string channel;
	string target;
	string		res;
	log_level	type;
	istringstream stream(cmd.arguments);

	stream >> channel >> target;

	if (channel.empty() || target.empty())
	{
		res = "Invalid number of arguments";
		type = ERROR;
		log(type, cmd.command, res);
		return (res);
	}
	if (!user.getIsOperator())
	{
		res = "No permission for this command";
		type = ERROR;
		log(type, cmd.command, res);
		return (res);
	}
	std::map<string, Channel>::iterator it = findChannel(channel);
	if (it != channels.end())
	{
		res = "Channel not found";
		type = ERROR;
		log(type, cmd.command, res);
		return (res);
	}
	std::optional<std::map<string, User>::iterator> it2 = it->second.findUser(target);
	if (!it2)
	{
		res = "User not found in channel";
		type = ERROR;
		log(type, cmd.command, res);
		return (res);
	}
	it->second.removeUser(user.getUsername());
	res = "Kicked user from channel";
	type = INFO;
	log(type, cmd.command, res);
	return (res);
}

string	Server::_processMODE(cmd cmd, User &user) 
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
        res = "Invalid number of arguments";
        type = ERROR;
        log(type, cmd.command, res);
        return (res);
    }
	if (!user.getIsOperator())
	{
		res = "No permission for this command";
		type = ERROR;
		log(type, cmd.command, res);
		return (res);
	}
	std::map<string, Channel>::iterator it = findChannel(channel);
	if (it != channels.end())
	{
		res = "Channel not found";
		type = ERROR;
		log(type, cmd.command, res);
		return (res);
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
        return (res);
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
        return (res);
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
        return (res);
	}
	if (mode == "-o")
	{
		
		if (it->second.findUser(extra))
		{
            const User *opp =  getUser(extra);
			it->second.addOperator(*opp);
		}

        res = "Added operator";
        type = INFO;
        log(type, cmd.command, res);
        return (res);
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
        return (res);
	}
	else
	{
		res = "Command not found";
		type = ERROR;
		log(type, cmd.command, res);
		return (res);
	}
}
	

string	Server::_processINVITE(cmd cmd, User &user)
{
	string target;
	string channel;
	string		res;
	log_level	type;
	istringstream stream(cmd.arguments);

	stream >> channel >> target;
	
	if (channel.empty() || target.empty())
    {
        res = "Invalid number of arguments";
        type = ERROR;
        log(type, cmd.command, res);
        return (res);
    }
	if (!user.getIsOperator())
	{
		res = "No permission for this command";
		type = ERROR;
		log(type, cmd.command, res);
		return (res);
	}
	std::map<string, Channel>::iterator it = findChannel(target);
	if (it != channels.end())
	{
		res = "Channel not found";
		type = ERROR;
		log(type, cmd.command, res);
		return (res);
	}
	const User *invited = getUser(target);
	if (!invited)
	{
		res = "User not found";
		type = ERROR;
		log(type, cmd.command, res);
		return (res);
	}
	//sendInvite();
	it->second.addUser(invited->getNickname(), *invited);
	res = "Invite send to user";
	type = INFO;
	log(type, cmd.command, res);
	return (res);

}