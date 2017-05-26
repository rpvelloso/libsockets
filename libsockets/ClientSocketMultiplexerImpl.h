/*
 * ClientSocketMultiplexerImpl.h
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#ifndef CLIENTSOCKETMULTIPLEXERIMPL_H_
#define CLIENTSOCKETMULTIPLEXERIMPL_H_

class ClientSocketMultiplexerImpl {
public:
	ClientSocketMultiplexerImpl();
	virtual ~ClientSocketMultiplexerImpl();
	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket) = 0;
	virtual void multiplex() = 0;
	virtual void cancel() = 0;
	virtual size_t clientCount() = 0;
};

#endif /* CLIENTSOCKETMULTIPLEXERIMPL_H_ */
