/*
 * ChatClient.cpp
 *
 *  Created on: 20 de jul de 2018
 *      Author: rvelloso
 */

#include <sstream>
#include <algorithm>
#include "ChatClient.h"

const std::string serverName = "Server";

void toUpper(std::string &inp) {
	std::transform(inp.begin(), inp.end(), inp.begin(), ::toupper);
}

std::string toUpper(const std::string &str) {
	auto inp(str);
	std::transform(inp.begin(), inp.end(), inp.begin(), ::toupper);
	return inp;
}

ChatMessage ChatClient::processCmd(const std::string& commandLine, std::ostream &outp) {
	ChatMessage result;
	std::stringstream ss(commandLine);
	std::string command, param, message;

	ss >> command;
	toUpper(command);
	ss >> param;
	std::getline(ss, message);

	std::cerr << "\'" << command << "\', \'" << param << "\', \'" << message << "\'" << std::endl;

	if (command == "NICK")
		result = nickCmd(param);
	else if (command == "SEND")
		result = sendCmd(param, message);
	else if (command == "LIST")
		result = listCmd();
	else if (command == "PING")
		result = pingCmd();
	else
		result = buildServerMessage("invalid command");

	processMessageQueue(outp);
	return result;
}

ChatMessage ChatClient::nickCmd(const std::string &newNick) {
	//TODO: add more nickname validation rules
	if (id != -1)
		return buildServerMessage("nickname already set");
	if (newNick.empty())
		return buildServerMessage("nickname can't be empty");
	if (toUpper(newNick) == toUpper(serverName))
		return buildServerMessage("reserved nickname");

	try {
		id = getMsgDb().addUser(newNick);
		nick = newNick;
		return buildServerMessage("nickname set");
	} catch (std::runtime_error &e) {
		return buildServerMessage("nickname already in use");
	} catch (std::exception &e) {
		return buildServerMessage(e.what());
	}
}

ChatMessage ChatClient::listCmd() {
	std::string nickList = "LIST = ";
	auto users = getMsgDb().retrieveUsers();
	if (users.size() > 0) {
		for (auto &user: users)
			nickList += user + ",";
		nickList.pop_back();
	}

	return buildServerMessage(nickList);
}

ChatMessage ChatClient::sendCmd(const std::string &receiver, const std::string &message) {
	if (id == -1)
		return buildServerMessage("nickname not set");
	else if (toUpper(receiver) == toUpper(serverName))
		return buildServerMessage("invalid receiver");
	else
		getMsgDb().sendMessage({
			nick,
			receiver,
			message,
			MessageType::PrivateMessage});
	return buildServerMessage("message sent");
}

ChatMessage ChatClient::pingCmd() {
	return buildServerMessage("PONG");
}

ChatMessage ChatClient::buildServerMessage(const std::string& msg) {
	return {serverName, "", msg, MessageType::ServerMessage};
}

MessageDB& ChatClient::getMsgDb() {
	static MessageDB msgDB;

	return msgDB;
}

void ChatClient::removeUser() {
	if (id != -1)
		getMsgDb().removeUser(id);
	id = -1;
	nick = "";
}

void ChatClient::processMessageQueue(std::ostream &outp) {
	auto messageQueue = getMsgDb().retrieveMessages(nick);
	for (auto &m: messageQueue)
		outp << m.toString() << std::endl;
}
