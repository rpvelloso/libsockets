/*
 * LinuxMultiplexer.h
 *
 *  Created on: 29 de mai de 2017
 *      Author: roberto
 */

#ifndef LINUXMULTIPLEXER_H_
#define LINUXMULTIPLEXER_H_

#include <vector>
#include <mutex>
#include <memory>
#include <functional>
#include <unordered_map>
#include "LinuxSocket.h"
#include "MultiplexerImpl.h"
#include "ClientSocket.h"

class LinuxMultiplexer: public MultiplexerImpl {
public:
	LinuxMultiplexer(MultiplexerCallback readCallback, MultiplexerCallback writeCallback);
	virtual ~LinuxMultiplexer();
	void addClientSocket(std::unique_ptr<ClientSocket> clientSocket) override;
	void addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
			std::shared_ptr<ClientData> clientData) override;
	void multiplex() override;
	void cancel() override;
	void interrupt();
	size_t clientCount() override;
private:
	std::unordered_map<int, std::shared_ptr<MultiplexedClientSocket>> clients;
	std::mutex clientsMutex, commandMutex;
	std::shared_ptr<ClientSocket> sockIn;
	int sockOutFD;

	void sendMultiplexerCommand(int cmd);
};

#endif /* LINUXMULTIPLEXER_H_ */
