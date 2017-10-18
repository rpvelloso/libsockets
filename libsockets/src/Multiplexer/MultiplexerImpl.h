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

#include "Socket/ClientSocket.h"

namespace socks {

class Poll;
class BufferedClientSocket;

using ClientListType = std::unordered_map<SocketFDType, std::unique_ptr<BufferedClientSocket>>;

class MultiplexerImpl {
public:
	MultiplexerImpl(Poll *pollStrategy,
			ClientCallback readCallbackFunc = defaultCallback,
			ClientCallback connectCallbackFunc = defaultCallback,
			ClientCallback disconnectCallbackFunc = defaultCallback,
			ClientCallback writeCallbackFunc = defaultCallback);
	virtual ~MultiplexerImpl();
	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket);
	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
			ClientCallback readCallbackFunc,
			ClientCallback connectCallbackFunc = defaultCallback,
			ClientCallback disconnectCallbackFunc = defaultCallback,
			ClientCallback writeCallbackFunc = defaultCallback);
	virtual size_t getClientCount();

	virtual void cancel();
	virtual void interrupt();
	virtual void multiplex();
protected:
	std::unique_ptr<Poll> pollStrategy;
	std::unique_ptr<ClientSocket> sockIn;
	std::mutex commandMutex, incomingClientsMutex;
	ClientCallback readCallbackFunc, connectCallbackFunc, disconnectCallbackFunc, writeCallbackFunc;
	ClientListType clients;
	std::atomic<size_t> clientCount;
	std::vector<std::unique_ptr<BufferedClientSocket>> incomingClients;
	SocketFDType sockOutFD = InvalidSocketFD;

	virtual void sendMultiplexerCommand(int cmd);
	virtual void removeClientSocket(BufferedClientSocket &clientSocket);
	virtual bool selfPipe(BufferedClientSocket &clientSocket);

	/*
	 * Factory method that wraps a clientSocket with a multiplexing interface
	 */
	std::unique_ptr<BufferedClientSocket> makeMultiplexed(
			std::unique_ptr<ClientSocket> clientSocket,
			ClientCallback readCallbackFunc,
			ClientCallback connectCallbackFunc,
			ClientCallback disconnectCallbackFunc,
			ClientCallback writeCallbackFunc);

	bool readHandler(BufferedClientSocket &client);
	bool writeHandler(BufferedClientSocket &client);
};

}
#endif /* SRC_MULTIPLEXER_MULTIPLEXERIMPL_H_ */
