/*
 * LinuxMultiplexer.h
 *
 *  Created on: 29 de mai de 2017
 *      Author: roberto
 */

#ifndef LINUXMULTIPLEXER_H_
#define LINUXMULTIPLEXER_H_

#include <vector>
#include <mutex>
#include <memory>
#include <functional>
#include <unordered_map>
#include "LinuxSocket.h"
#include "MultiplexerImpl.h"
#include "ClientSocket.h"

class LinuxMultiplexer: public MultiplexerImpl {
public:
	LinuxMultiplexer(
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback,
			MultiplexerCallback disconnectCallback,
			MultiplexerCallback writeCallback);
	virtual ~LinuxMultiplexer();
protected:
	std::vector<pollTuple> pollClients() override;
};

#endif /* LINUXMULTIPLEXER_H_ */
