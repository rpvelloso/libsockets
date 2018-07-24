/*
 * ChatClient.h
 *
 *  Created on: 20 de jul de 2018
 *      Author: rvelloso
 */

#ifndef CHATCLIENT_H_
#define CHATCLIENT_H_

#include <iostream>

#include "ChatMessage.h"
#include "MessageDB.h"

class ChatClient {
public:
	ChatMessage processCmd(const std::string &, std::ostream &);
	void processMessageQueue(std::ostream &outp);
	ChatMessage buildServerMessage(const std::string &msg);

	void removeUser();
	static MessageDB& getMsgDb();

private:
	int id = -1;
	std::string nick;

	ChatMessage nickCmd(const std::string &newNick);
	ChatMessage listCmd();
	ChatMessage pingCmd();
	ChatMessage sendCmd(const std::string &receiver, const std::string &message);
};

#endif /* CHATCLIENT_H_ */
