/*
 * MultiplexedClientSocket.h
 *
 *  Created on: 29 de mai de 2017
 *      Author: rvelloso
 */

#ifndef MULTIPLEXEDCLIENTSOCKET_H_
#define MULTIPLEXEDCLIENTSOCKET_H_

#include "ClientSocket.h"
#include "MultiplexerImpl.h"

class MultiplexerImpl;

/*
 * Wrapper/Decorator class. Adds multiplexing capabilities to a ClientSocket.
 * Client can signal the multiplexer if there is data to be sent.
 */
class MultiplexedClientSocket: public ClientSocket {
public:
	MultiplexedClientSocket(std::shared_ptr<SocketImpl> impl, std::function<void()> interruptFunc);
	virtual ~MultiplexedClientSocket();
	bool getHasOutput();
	void setHasOutput(bool hasOutput);
	void interrupt();
private:
	bool hasOutput = false;
	std::function<void()> interruptFunc;
};

#endif /* MULTIPLEXEDCLIENTSOCKET_H_ */
