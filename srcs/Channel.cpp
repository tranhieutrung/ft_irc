/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttero <ttero@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 11:47:53 by ttero             #+#    #+#             */
/*   Updated: 2025/03/17 19:55:43 by ttero            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"


std::map<int, User>::iterator Channel:: findUser (int user_fd)
{
	return (UserList.find(user_fd));
}

void Channel::kickUser(int user_fd)
{
	std::map<int, User>::iterator it = findUser(user_fd);
	if (it != UserList.end())
	{
		UserList.erase(it);
	}
	else
		std::cout << "User not found\n";

}

void Channel::changeTopic(int user_fd, string new_topic)
{
	std::map<int, User>::iterator it = findUser(user_fd);
	if (topic_restriction && !it->second.getIsOperator())
	{
		std::cout << "Non-operator can't change topic\n";
	}
	else
	{
		this->ChannelTopic= new_topic;
	}
}


