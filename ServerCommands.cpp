
#include "Server.hpp"

std::map<string, Channel>::iterator Server:: findChannel (string channel)
{
	return (channels.find(channel));
}


string	Server::_processTOPIC(cmd cmd, User &user) 

{	
	string		res;
	log_level	type;

	if (!cmd.arguments || cmd.arguments->size() < 2)
	{
		res = "Invalid number of arguments";
		type = ERROR;
		log(type, cmd.command, res);
		return (res);
	}
	std::map<string, Channel>::iterator it = findChannel(cmd.arguments[0]);
	if (it != channels.end())
	{
		res = "Channel not found";
		type = ERROR;
		log(type, cmd.command, res);
		return (res);
	}
	if (cmd.arguments->size() == 2)
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
		it->second.setChannelTopic(cmd.arguments[1]);
	}
	res = "Changed topic for channel";
	type = INFO;
	log(type, cmd.command, res);
	return (res);
	
}



string	Server::_processKICK(cmd cmd, User &user) 
{
	string		res;
	log_level	type;

	if (!cmd.arguments || cmd.arguments->size() < 2)
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
	std::map<string, Channel>::iterator it = findChannel(cmd.arguments[0]);
	if (it != channels.end())
	{
		res = "Channel not found";
		type = ERROR;
		log(type, cmd.command, res);
		return (res);
	}
	std::optional<std::map<string, User>::iterator> it2 = it->second.findUser(cmd.arguments[1]);
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
	string		res;
	log_level	type;

	if (!cmd.arguments || cmd.arguments->size() < 2)
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
	std::map<string, Channel>::iterator it = findChannel(cmd.arguments[0]);
	if (it != channels.end())
	{
		res = "Channel not found";
		type = ERROR;
		log(type, cmd.command, res);
		return (res);
	}
	if (cmd.arguments[1] == "-i")
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
	if (cmd.arguments[1] == "-t")
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
	if (cmd.arguments[1] == "-k")
	{
		if (cmd.arguments->size() == 3)
		{
            it->second.setPassword(nullptr);
		}
        if (cmd.arguments->size() == 3)
		{
            it->second.setPassword(cmd.arguments[2]);
		}
        res = "Switched password";
        type = INFO;
        log(type, cmd.command, res);
        return (res);
	}
	if (cmd.arguments[1] == "-o")
	{
		if (it->second.findUser(cmd.arguments[2]))
		{
            it->second.addOperator(cmd.arguments[2]);
		}
        res = "Added operator";
        type = INFO;
        log(type, cmd.command, res);
        return (res);
	}
	if (cmd.arguments[1] == "-l")
	{
		if (cmd.arguments->size() == 2)
		{
			it->second.setUserLimit(999);
		}
		if (cmd.arguments->size() == 3)
		{
			it->second.setUserLimit(stoi(cmd.arguments[2]));
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
	