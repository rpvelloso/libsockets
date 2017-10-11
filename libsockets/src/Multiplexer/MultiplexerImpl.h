/*
    Copyright 2017 Roberto Panerai Velloso.
    This file is part of libsockets.
    libsockets is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    libsockets is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with libsockets.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SRC_MULTIPLEXER_MULTIPLEXERIMPL_H_
#define SRC_MULTIPLEXER_MULTIPLEXERIMPL_H_

#include <tuple>
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <functional>
#include <unordered_map>

#include "Multiplexer/MultiplexedClientSocket.h"

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
#endif /* SRC_MULTIPLEXER_MULTIPLEXERIMPL_H_ */
