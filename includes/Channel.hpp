

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
            map<int, User> UserList;
            bool        inviteOnly;
            bool        topic_restriction;

    public:

        Channel(const std::string& name) : ChannelName(name), inviteOnly(false), topic_restriction(false) {}

        // Getters
        std::string getChannelName() const { return ChannelName; }
        std::string getChannelTopic() const { return ChannelTopic; }
        std::string getPassword() const { return password; }
        const std::map<int, User>& getUserList() const { return UserList; }
        bool isInviteOnly() const { return inviteOnly; }
        bool isTopicRestricted() const { return topic_restriction; }

        // Setters
        void setChannelName(const std::string& name) { ChannelName = name; }
        void setChannelTopic(const std::string& topic) { ChannelTopic = topic; }
        void setPassword(const std::string& pass) { password = pass; }
        void setInviteOnly(bool status) { inviteOnly = status; }
        void setTopicRestriction(bool status) { topic_restriction = status; }

        // User management
        void addUser(const int& user_fd, const User& user) { UserList[user_fd] = user; }
        void removeUser(int& user_fd) { UserList.erase(user_fd); }
};




#endif
