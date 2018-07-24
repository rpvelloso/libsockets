/*
 * MessageDB.cpp
 *
 *  Created on: 24 de jul de 2018
 *      Author: rvelloso
 */

#include "MessageDB.h"

extern std::string toUpper(const std::string &str);

MessageDB::MessageDB() : db(":memory:") {
	db.createQuery(
			"create table user ("
			"id integer primary key, "
			"nick varchar, "
			"ts datetime default current_timestamp not null, "
			"UNIQUE (nick COLLATE NOCASE));").execute();

	db.createQuery(
			"create table message ("
			"id integer primary key, "
			"sender integer, "
			"receiver integer, "
			"body varchar, "
			"type integer, "
			"ts datetime default current_timestamp not null, "
			"constraint fk_receiver "
			"foreign key(receiver) "
			"references user(id) "
			"on delete cascade "
			"constraint fk_sender "
			"foreign key(sender) "
			"references user(id) "
			"on delete cascade);").execute();

	db.createQuery(
			"create index receiverIndex "
			"on message(receiver);").execute();
}

std::vector<ChatMessage> MessageDB::retrieveMessages(const std::string &receiver) {
	std::vector<ChatMessage> messages;
	std::string sender, message;
	int type, id;
	std::string idList;

	auto results = db.createQuery(
			"select msg.id, snd.nick, msg.body, msg.type "
			"from message msg, user snd, user rcv "
			"where msg.sender = snd.id "
			"and msg.receiver = rcv.id "
			"and upper(rcv.nick) = ? "
			"order by msg.ts asc", toUpper(receiver)).getResult();

	while (results.next()) {
		results.fetch(id, sender, message, type);
		messages.push_back({
			sender,
			receiver,
			message,
			(MessageType)type});
		idList += std::to_string(id) + ",";
	}

	if (messages.size() > 0) {
		idList.pop_back();
		db.createQuery("delete from message where id in (" + idList + ")").execute();
	}
	return messages;
}

void MessageDB::sendMessage(ChatMessage msg) {
	auto sender = getUserID(msg.getSender());
	auto receiver = getUserID(msg.getReceiver());

	if (sender == -1)
		throw std::invalid_argument("nick " + msg.getSender() + "not found");
	if (receiver == -1)
		throw std::invalid_argument("nick " + msg.getReceiver() + "not found");

	db.createQuery(
			"insert into message "
			"(sender, receiver, body, type) "
			"values (?, ?, ?, ?)",
			sender, receiver,
			msg.getMessage(),
			(int) msg.getMessageType()).execute();
}

int MessageDB::getUserID(const std::string &nick) {
	int id;
	auto result = db.createQuery(
			"select id from user where upper(nick) = ?", toUpper(nick)).getResult();
	if (result.next()) {
		result.fetch(id);
		return id;
	}
	return  -1;
}

void MessageDB::removeUser(int id) {
	db.createQuery("delete from user where id = ?", id).execute();
}

int MessageDB::addUser(const std::string &nick) {
	db.createQuery("insert into user (nick) values (?)", nick).execute();
	return db.lastInsertRowID();
}

std::vector<std::string> MessageDB::retrieveUsers() {
	std::vector<std::string> users;
	auto results = db.createQuery("select nick from user").getResult();
	while (results.next()) {
		std::string user;
		results.fetch(user);
		users.push_back(user);
	}
	return users;
}
