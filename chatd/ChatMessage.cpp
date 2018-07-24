/*
 * ChatMessage.cpp
 *
 *  Created on: 23 de jul de 2018
 *      Author: rvelloso
 */

#include "ChatMessage.h"

ChatMessage::ChatMessage(
		const std::string &sender,
		const std::string &receiver,
		const std::string &message,
		MessageType messageType) :
		sender(sender),
		receiver(receiver),
		message(message),
		messageType(messageType) {
}

const std::string& ChatMessage::getMessage() const {
	return message;
}

void ChatMessage::setMessage(const std::string& message) {
	this->message = message;
}

const std::string& ChatMessage::getReceiver() const {
	return receiver;
}

void ChatMessage::setReceiver(const std::string& receiver) {
	this->receiver = receiver;
}

const std::string& ChatMessage::getSender() const {
	return sender;
}

void ChatMessage::setSender(const std::string& sender) {
	this->sender = sender;
}

MessageType ChatMessage::getMessageType() const {
	return messageType;
}

void ChatMessage::setMessageType(MessageType messageType) {
	this->messageType = messageType;
}

std::string ChatMessage::toString() {
	return "FROM " + sender + " " + message;
}
