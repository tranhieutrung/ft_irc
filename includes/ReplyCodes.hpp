#pragma once

#define	RPL_WELCOME 001
// "Welcome to the Internet Relay Network
//               <nick>!<user>@<host>"

#define	RPL_YOURHOST 002
// "Your host is <servername>, running version <ver>"

#define	RPL_CREATED 003
// "This server was created <date>"

#define	RPL_MYINFO 004
// "<servername> <version> <available user modes> <available channel modes>"

#define	RPL_TOPIC 332
// "<channel> :<topic>"
// When sending a TOPIC message to determine the channel topic, one of two 
// replies is sent.  If the topic is set, RPL_TOPIC is sent back else RPL_NOTOPIC.

#define RPL_AWAY 301
// "<nick> :<away message>"

#define RPL_NAMREPLY 353
// "( "=" / "*" / "@" ) <channel>
//:[ "@" / "+" ] <nick> *( " " [ "@" / "+" ] <nick> )
// - "@" is used for secret channels, "*" for private
//  channels, and "=" for others (public channels).

#define RPL_WHOISUSER 311
//              "<nick> <user> <host> * :<real name>"

#define RPL_PONG 399
// define for replying PING commands
// PONG message