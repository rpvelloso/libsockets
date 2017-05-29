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
	WindowsMultiplexer(MultiplexerCallback callback);
	virtual ~WindowsMultiplexer();
	void addClientSocket(std::unique_ptr<ClientSocket> clientSocket) override;
	void addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
			MultiplexerCallback customCallback) override;
	void multiplex() override;
	void cancel() override;
	size_t clientCount() override;
private:
	std::unordered_map<SOCKET, std::shared_ptr<ClientSocket>> clients;
	std::unordered_map<SOCKET, MultiplexerCallback> customCallback;
	std::mutex clientsMutex, commandMutex;
	std::shared_ptr<ClientSocket> sockIn;
	SOCKET sockOutFD;

	void sendMultiplexerCommand(int cmd);
	void interrupt();
	void removeClientSocket(SOCKET fd);
};

#endif /* WINDOWSMULTIPLEXER_H_ */
