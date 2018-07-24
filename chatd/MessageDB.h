/*
 * MessageDB.h
 *
 *  Created on: 24 de jul de 2018
 *      Author: rvelloso
 */

#ifndef MESSAGEDB_H_
#define MESSAGEDB_H_

#include <vector>

#include "sqlite.c++.h"
#include "ChatMessage.h"

class MessageDB {
public:
	MessageDB();
	std::vector<ChatMessage> retrieveMessages(const std::string &receiver);
	std::vector<std::string> retrieveUsers();
	void sendMessage(ChatMessage);
	int getUserID(const std::string &nick);
	void removeUser(int id);
	int addUser(const std::string &nick);
private:
	SQLiteDB db;
};

#endif /* MESSAGEDB_H_ */

