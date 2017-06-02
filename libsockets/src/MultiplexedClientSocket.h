/*
 * MultiplexedClientSocket.h
 *
 *  Created on: 29 de mai de 2017
 *      Author: rvelloso
 */

#ifndef MULTIPLEXEDCLIENTSOCKET_H_
#define MULTIPLEXEDCLIENTSOCKET_H_

#include <sstream>
#include "ClientSocket.h"
#include "MultiplexerImpl.h"

class MultiplexerImpl;

/*
 * memento class with specific client data/state
 */
class ClientData {
public:
	ClientData() {};
	virtual ~ClientData() {};
};

/*
 * Wrapper/Decorator class. Adds multiplexing capabilities to a ClientSocket.
 * Client can signal the multiplexer if there is data to be sent and store client data/state.
 */

class MultiplexedClientSocket {
public:
	MultiplexedClientSocket(std::unique_ptr<ClientSocket> impl,
			std::function<void()> interruptFunc) : impl(std::move(impl)), interruptFunc(interruptFunc) {};
	virtual ~MultiplexedClientSocket() {};
	bool getHasOutput() { return hasOutput; };
	void setHasOutput(bool hasOutput) { this->hasOutput = hasOutput; };
	void interrupt() { interruptFunc(); };
	ClientData &getClientData() {return *clientData;};
	void setClientData(std::unique_ptr<ClientData> clientData) {this->clientData.reset(clientData.release());};
	bool getHangUp() {return hangUp;};
	void setHangUp(bool hangUp) {this->hangUp = hangUp;};
	std::stringstream &getOutputBuffer() {return outputBuffer;};
	std::stringstream &getInputBuffer() {return inputBuffer;};
	int receiveData(void *buf, size_t len) {return impl->receiveData(buf, len);};
	int sendData(const void *buf, size_t len) {return impl->sendData(buf, len);};
	size_t getSendBufferSize() {return impl->getSendBufferSize();};
	size_t getReceiveBufferSize() {return impl->getReceiveBufferSize();};
	SocketImpl &getImpl() {return impl->getImpl();};
private:
	std::unique_ptr<ClientSocket> impl;
	bool hasOutput = false;
	std::function<void()> interruptFunc;
	std::unique_ptr<ClientData> clientData;
	bool hangUp = false;
	std::stringstream outputBuffer;
	std::stringstream inputBuffer;
};

#endif /* MULTIPLEXEDCLIENTSOCKET_H_ */
