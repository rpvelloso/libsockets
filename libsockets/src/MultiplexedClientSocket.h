/*
 * MultiplexedClientSocket.h
 *
 *  Created on: 29 de mai de 2017
 *      Author: rvelloso
 */

#ifndef MULTIPLEXEDCLIENTSOCKET_H_
#define MULTIPLEXEDCLIENTSOCKET_H_

#include <iostream>

#include <sstream>
#include "ClientSocket.h"
#include "MultiplexerImpl.h"

namespace socks {

class MultiplexerImpl;

/*
 * memento base class to store specific client data/state
 */
class ClientData {
public:
	ClientData() {};
	virtual ~ClientData() {};
};

/*
 * Wrapper class. Adds multiplexing and buffering capabilities to a ClientSocket.
 * Signals the multiplexer if there is data to be sent and store client data/state.
 */

class MultiplexedClientSocket {
public:
	MultiplexedClientSocket(
			std::unique_ptr<ClientSocket> impl,
			std::unique_ptr<ClientData> clientData,
			std::function<void()> interruptFunc) :
				impl(std::move(impl)),
				clientData(std::move(clientData)),
				interruptFunc(interruptFunc) {};
	//virtual ~MultiplexedClientSocket() {};
	bool getHasOutput() { return outputBuffer.rdbuf()->in_avail() > 0; };
	void interrupt() { interruptFunc(); };
	ClientData &getClientData() {return *clientData;};
	std::stringstream &getOutputBuffer() {return outputBuffer;};
	std::stringstream &getInputBuffer() {return inputBuffer;};

	int receiveData(void *buf, size_t len) {return impl->receiveData(buf, len);};
	int sendData(const void *buf, size_t len) {return impl->sendData(buf, len);};
	size_t getSendBufferSize() const {return impl->getSendBufferSize();};
	size_t getReceiveBufferSize() const {return impl->getReceiveBufferSize();};
	SocketImpl &getImpl() {return impl->getImpl();};
private:
	std::unique_ptr<ClientSocket> impl;
	std::unique_ptr<ClientData> clientData;
	std::function<void()> interruptFunc;
	std::stringstream outputBuffer;
	std::stringstream inputBuffer;
};

}
#endif /* MULTIPLEXEDCLIENTSOCKET_H_ */
