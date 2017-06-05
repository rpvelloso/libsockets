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
	void addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
			std::unique_ptr<ClientData> clientData) override;
	size_t clientCount() override;
	void removeClientSocket(MultiplexedClientSocket &clientSocket) override;
	bool selfPipe(MultiplexedClientSocket &clientSocket) override;
	virtual std::vector<pollTuple> pollClients() override;

private:
	std::unordered_map<SocketFDType, std::unique_ptr<MultiplexedClientSocket>> clients;
	SocketFDType sockOutFD;
};

#endif /* LINUXMULTIPLEXER_H_ */
