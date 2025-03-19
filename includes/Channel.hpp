#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <poll.h>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <exception>
#include <optional>

// Forward declaration of User
class User;

using namespace std;

class Channel
{
    private:
            string ChannelName;
            string ChannelTopic;
            string password;
            map<string, User> UserList;  // Using 'User' here will now work because of the forward declaration
            bool        inviteOnly;
            bool        topic_restriction;
            vector<string>      operators;
            unsigned int        userLimit;

    public:
        Channel() : ChannelName(""), ChannelTopic(""), password(""), inviteOnly(false), topic_restriction(false), userLimit(999) {}
        Channel(const std::string& name) : ChannelName(name), inviteOnly(false), topic_restriction(false) {}

        // Getters
        std::string getChannelName() const { return ChannelName; }
        std::string getChannelTopic() const { return ChannelTopic; }
        std::string getPassword() const { return password; }
        const std::map<string, User>& getUserList() const { return UserList; }
        bool isInviteOnly() const { return inviteOnly; }
        bool isTopicRestricted() const { return topic_restriction; }
        unsigned int  getUserLimit() const { return userLimit; }

        // Setters
        void setChannelName(const std::string& name) { ChannelName = name; }
        void setChannelTopic(const std::string& topic) { ChannelTopic = topic; }
        void setPassword(const std::string& pass) { password = pass; }
        void setInviteOnly(bool status) { inviteOnly = status; }
        void setTopicRestriction(bool status) { topic_restriction = status; }
        void setUserLimit(const unsigned int limit) {userLimit = limit;}

        // User management
        void addUser(const string& username, const User& user) ;//{ UserList[username] = user; }
        void removeUser(string user); //{ UserList.erase(user);};
        std::optional<std::map<string, User>::iterator> findUser(string username);
        void addOperator(const string& nick) {
                if (find(operators.begin(), operators.end(), nick) == operators.end()) {
                    operators.push_back(nick);
                }
            }
        void removeOperator(const string& nick) {
                operators.erase(remove(operators.begin(), operators.end(), nick), operators.end());
        }
        bool isOperator(const string& nick) const {
                return find(operators.begin(), operators.end(), nick) != operators.end();
            }

};

#endif