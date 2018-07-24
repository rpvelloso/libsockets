/*
 * ChatMessage.h
 *
 *  Created on: 23 de jul de 2018
 *      Author: rvelloso
 */

#ifndef CHATMESSAGE_H_
#define CHATMESSAGE_H_

#include <string>

enum class MessageType {
	ServerMessage = 0,
	PrivateMessage = 1
};

class ChatMessage {
public:
	ChatMessage() = default;
	ChatMessage(
			const std::string &sender,
			const std::string &receiver,
			const std::string &message,
			MessageType messageType);
	const std::string& getMessage() const;
	void setMessage(const std::string& message);
	const std::string& getReceiver() const;
	void setReceiver(const std::string& receiver);
	const std::string& getSender() const;
	void setSender(const std::string& sender);
	MessageType getMessageType() const;
	void setMessageType(MessageType messageType);
	std::string toString();
private:
	std::string sender, receiver, message;
	MessageType messageType = MessageType::ServerMessage;
};

#endif /* CHATMESSAGE_H_ */
