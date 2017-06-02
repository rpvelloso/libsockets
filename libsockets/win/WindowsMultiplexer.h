/*
 * WindowsMultiplexer.h
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#ifndef WINDOWSMULTIPLEXER_H_
#define WINDOWSMULTIPLEXER_H_

#include <vector>
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
			std::unique_ptr<ClientData> clientData) override;
	size_t clientCount() override;
protected:
	void removeClientSocket(MultiplexedClientSocket &clientSocket) override;
	bool selfPipe(MultiplexedClientSocket &clientSocket) override;
	std::vector<pollTuple> pollClients() override;

private:
	std::unordered_map<SOCKET, std::unique_ptr<MultiplexedClientSocket>> clients;
	SOCKET sockOutFD;
};

#endif /* WINDOWSMULTIPLEXER_H_ */
