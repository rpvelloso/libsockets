/*
 * WindowsMultiplexer.h
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#ifndef WINDOWSMULTIPLEXER_H_
#define WINDOWSMULTIPLEXER_H_

#include <vector>
#include <mutex>
#include <memory>
#include "MultiplexerImpl.h"
#include "ClientSocket.h"

class WindowsMultiplexer: public MultiplexerImpl {
public:
	WindowsMultiplexer();
	virtual ~WindowsMultiplexer();
	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket);
	virtual void multiplex();
	virtual void cancel();
	virtual size_t clientCount();
private:
	std::vector<std::unique_ptr<ClientSocket>> clients;
	std::mutex clientsMutex;
};

#endif /* WINDOWSMULTIPLEXER_H_ */
