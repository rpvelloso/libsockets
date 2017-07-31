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
#include <atomic>
#include <functional>
#include <unordered_map>
#include "MultiplexedClientSocket.h"

namespace socks {

class Poll;
class MultiplexedClientSocket;

using ClientListType = std::unordered_map<SocketFDType, std::unique_ptr<MultiplexedClientSocket>>;

extern MultiplexerCallback defaultCallback;

class MultiplexerImpl {
public:
	MultiplexerImpl(Poll *pollStrategy,
			MultiplexerCallback readCallbackFunc = defaultCallback,
			MultiplexerCallback connectCallbackFunc = defaultCallback,
			MultiplexerCallback disconnectCallbackFunc = defaultCallback,
			MultiplexerCallback writeCallbackFunc = defaultCallback);
	virtual ~MultiplexerImpl();
	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
			std::unique_ptr<ClientData> clientData);
	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
			std::unique_ptr<ClientData> clientData,
			MultiplexerCallback readCallbackFunc,
			MultiplexerCallback connectCallbackFunc = defaultCallback,
			MultiplexerCallback disconnectCallbackFunc = defaultCallback,
			MultiplexerCallback writeCallbackFunc = defaultCallback);
	virtual size_t getClientCount();

	virtual void cancel();
	virtual void interrupt();
	virtual void multiplex();
protected:
	std::unique_ptr<Poll> pollStrategy;
	std::unique_ptr<ClientSocket> sockIn;
	std::mutex commandMutex, incomingClientsMutex;
	MultiplexerCallback readCallbackFunc, connectCallbackFunc, disconnectCallbackFunc, writeCallbackFunc;
	ClientListType clients;
	std::atomic<size_t> clientCount;
	std::vector<std::unique_ptr<MultiplexedClientSocket>> incomingClients;
	SocketFDType sockOutFD = InvalidSocketFD;

	virtual void sendMultiplexerCommand(int cmd);
	virtual void removeClientSocket(MultiplexedClientSocket &clientSocket);
	virtual bool selfPipe(MultiplexedClientSocket &clientSocket);

	/*
	 * Factory method that wraps a clientSocket with a multiplexing interface
	 */
	std::unique_ptr<MultiplexedClientSocket> makeMultiplexed(
			std::unique_ptr<ClientSocket> clientSocket,
			std::unique_ptr<ClientData> clientData,
			MultiplexerCallback readCallbackFunc,
			MultiplexerCallback connectCallbackFunc,
			MultiplexerCallback disconnectCallbackFunc,
			MultiplexerCallback writeCallbackFunc);

	bool readHandler(MultiplexedClientSocket &client);
	bool writeHandler(MultiplexedClientSocket &client);
};

}
#endif /* MULTIPLEXERIMPL_H_ */
