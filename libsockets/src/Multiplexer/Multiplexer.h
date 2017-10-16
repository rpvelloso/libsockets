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

#ifndef SRC_MULTIPLEXER_MULTIPLEXER_H_
#define SRC_MULTIPLEXER_MULTIPLEXER_H_

#include <thread>
#include <memory>

#include "Socket/ClientSocket.h"
#include "Multiplexer/MultiplexerImpl.h"

namespace socks {
/*
 * AKA Reactor Pattern - https://en.wikipedia.org/wiki/Reactor_pattern
 */

class Multiplexer {
public:
	Multiplexer() = delete;
	Multiplexer(Multiplexer &&) = default;
	Multiplexer &operator=(Multiplexer &&) = default;

	Multiplexer(MultiplexerImpl *impl);
	~Multiplexer();
	void addClientSocket(std::unique_ptr<ClientSocket> clientSocket);
	void addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback = defaultCallback,
			MultiplexerCallback disconnectCallback = defaultCallback,
			MultiplexerCallback writeCallback = defaultCallback);

	void multiplex();
	void cancel();
	void interrupt();
	size_t getClientCount();
protected:
	std::unique_ptr<MultiplexerImpl> impl;
	std::unique_ptr<std::thread> thread;
};

namespace factory {
	Multiplexer makeMultiplexer(
		MultiplexerCallback readCallbackFunc = defaultCallback,
		MultiplexerCallback connectCallbackFunc = defaultCallback,
		MultiplexerCallback disconnectCallbackFunc = defaultCallback,
		MultiplexerCallback writeCallbackFunc = defaultCallback);
}

}
#endif /* SRC_MULTIPLEXER_MULTIPLEXER_H_ */
