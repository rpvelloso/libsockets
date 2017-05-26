/*
 * ClientSocketMultiplexerImpl.h
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#ifndef MULTIPLEXERIMPL_H_
#define MULTIPLEXERIMPL_H_

class MultiplexerImpl {
public:
	MultiplexerImpl();
	virtual ~MultiplexerImpl();
	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket) = 0;
	virtual void multiplex() = 0;
	virtual void cancel() = 0;
	virtual size_t clientCount() = 0;
};

#endif /* MULTIPLEXERIMPL_H_ */
