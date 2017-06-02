/*
 * ClientSocketMultiplexerImpl.h
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#ifndef MULTIPLEXERIMPL_H_
#define MULTIPLEXERIMPL_H_

#include <tuple>
#include <memory>
#include <vector>
#include <mutex>
#include <functional>
#include "MultiplexedClientSocket.h"

class MultiplexedClientSocket;

using MultiplexerCallback = std::function<void(std::stringstream &inp, std::stringstream &outp, ClientData&)>;
using pollTuple = std::tuple<MultiplexedClientSocket &, bool, bool>;

class MultiplexerImpl {
public:
	MultiplexerImpl(MultiplexerCallback callback);
	virtual ~MultiplexerImpl();
	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket) = 0;
	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
			std::unique_ptr<ClientData> clientData) = 0;
	virtual size_t clientCount() = 0;

	virtual void cancel();
	virtual void interrupt();
	virtual void multiplex();


protected:
	std::unique_ptr<ClientSocket> sockIn;
	std::mutex commandMutex, clientsMutex;
	MultiplexerCallback callback;

	virtual void sendMultiplexerCommand(int cmd);

	virtual std::vector<pollTuple> pollClients() = 0;
	virtual void removeClientSocket(MultiplexedClientSocket &clientSocket) = 0;
	virtual bool selfPipe(MultiplexedClientSocket &clientSocket) = 0;

	/*
	 * Factory method that wraps a clientSocket with a multiplexing interface
	 */
	std::unique_ptr<MultiplexedClientSocket> makeMultiplexed(std::unique_ptr<ClientSocket> clientSocket);

	bool readHandler(MultiplexedClientSocket &client);
	bool writeHandler(MultiplexedClientSocket &client);
};

#endif /* MULTIPLEXERIMPL_H_ */
