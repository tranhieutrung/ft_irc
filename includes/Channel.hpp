

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <poll.h>
#include <iostream>
#include <map>
#include "User.hpp"

using namespace std;

class Channel
{
    private:
            string ChannelName;
            string ChannelTopic;
            string password;
            map<std::string, User> UserList;
            bool        inviteOnly;
            bool        topic_restriction;

    public:

        Channel(const std::string& name) : ChannelName(name), inviteOnly(false), topic_restriction(false) {}

        // Getters
        std::string getChannelName() const { return ChannelName; }
        std::string getChannelTopic() const { return ChannelTopic; }
        std::string getPassword() const { return password; }
        const std::map<std::string, User>& getUserList() const { return UserList; }
        bool isInviteOnly() const { return inviteOnly; }
        bool isTopicRestricted() const { return topic_restriction; }

        // Setters
        void setChannelName(const std::string& name) { ChannelName = name; }
        void setChannelTopic(const std::string& topic) { ChannelTopic = topic; }
        void setPassword(const std::string& pass) { password = pass; }
        void setInviteOnly(bool status) { inviteOnly = status; }
        void setTopicRestriction(bool status) { topic_restriction = status; }

        // User management
        void addUser(const std::string& nickname, const User& user) { UserList[nickname] = user; }
        void removeUser(const std::string& nickname) { UserList.erase(nickname); }
        
};




#endif
