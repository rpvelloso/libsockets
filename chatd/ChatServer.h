/*
 * ChatServer.h
 *
 *  Created on: 20 de jul de 2018
 *      Author: rvelloso
 */

#ifndef CHATSERVER_H_
#define CHATSERVER_H_

#include <libsockets-no-ssl.h>
#include "ChatClient.h"

class ChatServer {
public:
	ChatServer();
	virtual ~ChatServer();
	void start();
private:
	static std::string readline(std::istream &inp);
	void onConnect(socks::Context<ChatClient> &ctx, std::istream &inp, std::ostream &outp);
	void onDisconnect(socks::Context<ChatClient> &ctx, std::istream &inp, std::ostream &outp);
	void onReceive(socks::Context<ChatClient> &ctx, std::istream &inp, std::ostream &outp);
	socks::Server makeChatServer();
};

#endif /* CHATSERVER_H_ */
