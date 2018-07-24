/*
 * ChatServer.cpp
 *
 *  Created on: 20 de jul de 2018
 *      Author: rvelloso
 */

#include <functional>
#include "ChatServer.h"

ChatServer::ChatServer() {
}

ChatServer::~ChatServer() {
}

std::string ChatServer::readline(std::istream &inp) {
	std::string line;

	auto savePos = inp.tellg();
	std::getline(inp, line);
	if (inp && !inp.eof())
		return line;
	else
		inp.seekg(savePos);

	return "";
}

void ChatServer::onConnect(socks::Context<ChatClient> &ctx, std::istream &inp, std::ostream &outp) {
	outp << ctx.getContext().buildServerMessage("connected").toString() << std::endl;
}

void ChatServer::onDisconnect(socks::Context<ChatClient> &ctx, std::istream &inp, std::ostream &outp) {
	ctx.getContext().removeUser();
}

void ChatServer::onReceive(socks::Context<ChatClient> &ctx, std::istream &inp, std::ostream &outp) {
	ChatClient &context = ctx.getContext();

	while (inp) {
		auto cmd = readline(inp);
		if (!cmd.empty()) {
			outp << context.processCmd(cmd, outp).toString() << std::endl;
		} else
			break;
	}
}

void ChatServer::start() {
	auto server = makeChatServer();

	server.listen("127.0.0.1","10000");
}

socks::Server ChatServer::makeChatServer() {
	return socks::factory::makeThreadedServer<ChatClient>(
		std::bind(&ChatServer::onReceive, this,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3),
		std::bind(&ChatServer::onConnect, this,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3),
		std::bind(&ChatServer::onDisconnect, this,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3));
}
