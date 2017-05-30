/*
 * WindowsMultiplexer.h
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#ifndef WINDOWSMULTIPLEXER_H_
#define WINDOWSMULTIPLEXER_H_

#include <vector>
#include <mutex>
#include <memory>
#include <functional>
#include <unordered_map>
#include "WindowsSocket.h"
#include "MultiplexerImpl.h"
#include "ClientSocket.h"

class WindowsMultiplexer: public MultiplexerImpl {
public:
	WindowsMultiplexer(MultiplexerCallback readCallback, MultiplexerCallback writeCallback);
	virtual ~WindowsMultiplexer();
	void addClientSocket(std::unique_ptr<ClientSocket> clientSocket) override;
	void addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
			std::shared_ptr<ClientData> clientData) override;
	void multiplex() override;
	void cancel() override;
	void interrupt();
	size_t clientCount() override;
private:
	std::unordered_map<SOCKET, std::shared_ptr<MultiplexedClientSocket>> clients;
	std::mutex clientsMutex, commandMutex;
	std::shared_ptr<ClientSocket> sockIn;
	SOCKET sockOutFD;

	void sendMultiplexerCommand(int cmd);
};

#endif /* WINDOWSMULTIPLEXER_H_ */
