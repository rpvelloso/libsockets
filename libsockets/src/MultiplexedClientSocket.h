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

class MultiplexedClientSocket: public ClientSocket {
public:
	MultiplexedClientSocket(std::shared_ptr<SocketImpl> impl,
			std::function<void()> interruptFunc) : ClientSocket(impl), interruptFunc(interruptFunc) {};
	virtual ~MultiplexedClientSocket() {};
	bool getHasOutput() { return hasOutput; };
	void setHasOutput(bool hasOutput) { this->hasOutput = hasOutput; };
	void interrupt() { interruptFunc(); };
	std::shared_ptr<ClientData> getClientData() {return clientData;};
	void setClientData(std::shared_ptr<ClientData> clientData) {this->clientData = clientData;};
	bool getHangUp() {return hangUp;};
	void setHangUp(bool hangUp) {this->hangUp = hangUp;};
	std::stringstream &getOutputBuffer() {return outputBuffer;};
	std::stringstream &getInputBuffer() {return inputBuffer;};
private:
	bool hasOutput = false;
	std::function<void()> interruptFunc;
	std::shared_ptr<ClientData> clientData;
	bool hangUp = false;
	std::stringstream outputBuffer;
	std::stringstream inputBuffer;
};

#endif /* MULTIPLEXEDCLIENTSOCKET_H_ */
