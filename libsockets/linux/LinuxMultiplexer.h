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
	LinuxMultiplexer(MultiplexerCallback callback);
	virtual ~LinuxMultiplexer();
	void addClientSocket(std::unique_ptr<ClientSocket> clientSocket) override;
	void addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
			std::shared_ptr<ClientData> clientData) override;
	size_t clientCount() override;
	void removeClientSocket(std::shared_ptr<MultiplexedClientSocket> clientSocket) override;
	bool selfPipe(std::shared_ptr<MultiplexedClientSocket> clientSocket) override;
	std::unordered_map<std::shared_ptr<MultiplexedClientSocket>, std::pair<bool, bool>> pollClients() override;

private:
	std::unordered_map<int, std::shared_ptr<MultiplexedClientSocket>> clients;
	int sockOutFD;
};

#endif /* LINUXMULTIPLEXER_H_ */
