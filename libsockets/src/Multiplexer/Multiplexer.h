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
#include "Socket/SSL/OpenSSL.h"
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

	Multiplexer(MultiplexerImpl *impl) : impl(impl) {
		thread.reset(new std::thread([](MultiplexerImpl &multiplexer){
			multiplexer.multiplex();
			openSSL.threadCleanup();
		},
		std::ref(*(this->impl)) ));
	};

	virtual ~Multiplexer() {
		if (impl)
			impl->cancel();

		if (thread)
			thread->join();
	};

	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket) {
		impl->addClientSocket(std::move(clientSocket), std::make_unique<ClientData>());
	};
	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
			std::unique_ptr<ClientData> clientData) {
		impl->addClientSocket(std::move(clientSocket), std::move(clientData));
	};
	virtual void multiplex() {
		impl->multiplex();
	};
	virtual void cancel() {
		impl->cancel();
	};
	virtual void interrupt() {
		impl->interrupt();
	};
	virtual size_t getClientCount() {
		return impl->getClientCount();
	};
protected:
	std::unique_ptr<MultiplexerImpl> impl;
	std::unique_ptr<std::thread> thread;
};

}
#endif /* SRC_MULTIPLEXER_MULTIPLEXER_H_ */
