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

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>
#include "Factory/Poll.h"
#include "Socket/BufferedClientSocketInterface.h"
#include "Socket/ClientSocket.h"

namespace socks {

class MultiplexerImpl {
public:
	MultiplexerImpl(Poll *pollStrategy);
	virtual ~MultiplexerImpl();
	virtual void addClientSocket(std::unique_ptr<BufferedClientSocketInterface> clientSocket);
	virtual size_t getClientCount();

	virtual void cancel();
	virtual void interrupt();
	virtual void multiplex(int timeout = -1);
protected:
	std::unique_ptr<Poll> pollStrategy;
	std::unique_ptr<ClientSocket> sockIn;
	std::mutex commandMutex, incomingClientsMutex;
	ClientListType clients;
	std::atomic<size_t> clientCount;
	std::vector<std::unique_ptr<BufferedClientSocketInterface>> incomingClients;
	SocketFDType sockOutFD = InvalidSocketFD;

	void processIncomingClients();
	void sendMultiplexerCommand(int cmd);
	void removeClientSocket(BufferedClientSocketInterface &clientSocket);
	bool selfPipe(BufferedClientSocketInterface &clientSocket);

	bool readHandler(BufferedClientSocketInterface &client);
	bool writeHandler(BufferedClientSocketInterface &client);
};

}
#endif /* SRC_MULTIPLEXER_MULTIPLEXERIMPL_H_ */
